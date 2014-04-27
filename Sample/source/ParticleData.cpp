#include "stdafx.h"
#include "ParticleData.h"


//Trims all whitespace at the start/end of the in string
string ParticleData::Parser::trimWhitespace(string in) {
	int firstBeforeWhitespace = -1;
	int lastBitOfSpace = in.size();
	bool foundNonwhitespace = false;
	//Look for whitespace and mark what characters you've found
	for (unsigned int i = 0; i < in.size(); i++) {
		char c= in[i];
		if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n')) {
			if (!foundNonwhitespace)
				firstBeforeWhitespace = i;
		} 
		else {
			lastBitOfSpace = i;
			foundNonwhitespace = true;
		}
	}
	int start = firstBeforeWhitespace+1;
	int length = lastBitOfSpace - start+1;
	if (length <= 0)
		return "";
	return in.substr(start,length);
}

//Read a vector of floats into the appropriate type
//returns false if the vector of floats does not contain enough floats
//note: 1 float is a special case. 1 float always works because its just used for all components
bool ParticleData::Parser::readIn(vec4 & readInto, vector<float> values) {
	if (values.size() == 4)
		readInto = vec4(values[0],values[1],values[2],values[3]);
	else if (values.size() == 1)
		readInto = vec4(values[0],values[0],values[0],values[0]);
	else
		return false;
	return true;
}
bool ParticleData::Parser::readIn(vec3 & readInto, vector<float> values) {
	if (values.size() == 3)
		readInto = vec3(values[0],values[1],values[2]);
	else if (values.size() == 1)
		readInto = vec3(values[0],values[0],values[0]);
	else
		return false;
	return true;
}
bool ParticleData::Parser::readIn(vec2 & readInto, vector<float> values) {
	if (values.size() == 2)
		readInto = vec2(values[0],values[1]);
	else if (values.size() == 1)
		readInto = vec2(values[0],values[0]);
	else
		return false;
	return true;
}
bool ParticleData::Parser::readIn(float & readInto, vector<float> values) {
	if (values.size() == 1)
		readInto = values[0];
	else
		return false;
	return true;
}

//Check if the given value matches the name, if so apply it to the given property of the particle data
//if an error occurs, return false
#define ATTEMPT_PROCESS(name,property) if (valueName == name) {if (!readValue(property,value)) return false;break;}

//determine based off the name and section
//which interop chain the given value belongs too
//then use readValue to add the value to the chain
bool ParticleData::Parser::processValue(string valueName, string value) {
	//change valueName to lowercase
	for (unsigned int i = 0; i < valueName.size(); i++)
		valueName[i] = tolower(valueName[i]);

	switch (Section) {
	case SYSTEMRULES:
		ATTEMPT_PROCESS("generationrate",inProgress->GenerationRate)
		ATTEMPT_PROCESS("velocity",inProgress->Velocity)
		ATTEMPT_PROCESS("latitude",inProgress->Latitude)
		ATTEMPT_PROCESS("longitude",inProgress->Longitude)
		ATTEMPT_PROCESS("life",inProgress->Life)
		ATTEMPT_PROCESS("emittersize",inProgress->EmitterSize)
		ATTEMPT_PROCESS("frameoffset",inProgress->FrameOffset)
		ATTEMPT_PROCESS("velocityvariation",inProgress->VelocityVariation)
		ATTEMPT_PROCESS("scalevariation",inProgress->ScaleVariation)

		//If you got this far then it failed to be recognized
		cout << "Error on line " << lineNumber << ": Failed to recognize rule \"" << valueName << "\" as a valid rule name in section \"" << Section << "\"";
		return false;
	case PARTICLERULES:
		ATTEMPT_PROCESS("color",inProgress->Color)
		ATTEMPT_PROCESS("scale",inProgress->Scale)
		ATTEMPT_PROCESS("acceleration",inProgress->Acceleration)
		ATTEMPT_PROCESS("animationspeed",inProgress->AnimationSpeed)

		//If you got this far then it failed to be recognized
		cout << "Error on line " << lineNumber << ": Failed to recognize rule \"" << valueName << "\" as a valid rule name in section \"" << Section << "\"";
		return false;
	case NOSECTION:
		cout << "Error on line " << lineNumber << ": No section header before rule. All rules must be within a section.\n";
		return false;
    case PROPERTIES:
            break;
            
	}

	return true;
}

bool ParticleData::Parser::readProperty(string propertyName, string value) {
	//change propertyName to lowercase
	for (unsigned int i = 0; i < propertyName.size(); i++)
		propertyName[i] = tolower(propertyName[i]);

	if (propertyName == "materialstyle") {
		if (value == "NONE")
			inProgress->MaterialStyle = NONE;
		else if (value == "BLEND")
			inProgress->MaterialStyle = BLEND;
		else if (value == "ADDITIVE")
			inProgress->MaterialStyle = ADDITIVE;
		else if (value == "SCREEN")
			inProgress->MaterialStyle = SCREEN;
		else {
			cout << "Error on line " << lineNumber << ": Failed to recognize style \"" << value << "\" as a valid material style.\n";
			return false;
		}
	}
	else if (propertyName == "materialtexture") 
		inProgress->MaterialTexture = string("particles/textures/") + value;
	else if (propertyName == "rows") 
		inProgress->Rows = (float)atoi(value.c_str());
	else if (propertyName == "columns")
		inProgress->Columns = (float)atoi(value.c_str());
	else {
		cout << "Error on line " << lineNumber << ": Failed to recognize property \"" << propertyName << "\" as a valid property.\n";
		return false;
	}
	return true;
}

bool ParticleData::Parser::parseLine(string partOne, string partTwo, bool header) {
	if (header) {
		//check if the header is valid and change state
		if (partOne == "SystemRules")
			Section = SYSTEMRULES;
		else if (partOne == "ParticleRules")
			Section = PARTICLERULES;
		else if (partOne == "Properties")
			Section = PROPERTIES;
		else {
			cout << "Error on line " << lineNumber << ": Header: \"" << partOne << "\" is not recognized as a valid section header.\n";
			return false;
		}
		lastSectionName = partOne;
	}
	else if (Section != PROPERTIES) {
		//Determine the type of value
		//is it a singleton, or is it a list of values
		if (partTwo[0] == '{') {
			//It's a list of values, break up the list and then add each value in the list individually
			//Lets check the list has a terminator
			if (partTwo[partTwo.size()-1] != '}') {
				cout << "Error on line " << lineNumber << ": Missing list terminator '}'\n";
				return false;
			}
			string buffer;
			//Replace the last character with a ',' to force the last value to read
			partTwo[partTwo.size()-1] = ',';
			for (unsigned int i = 1; i < partTwo.size(); i++) {
				char c= partTwo[i];
				if (c == ',') {
					//Process the given value
					if (!processValue(partOne,trimWhitespace(buffer)))
						return false;
					buffer = "";
				}
				else
					buffer += c;
			}
			//All values read!
		}
		else {
			//Assume there's only a single value here, append a 0 time and go from there
			string value = "0:" + partTwo;
			if (!processValue(partOne,value))
				return false;
		}
	}
	else {
		if ((partTwo.size() < 2) || (partTwo[0] != '"') || (partTwo[partTwo.size()-1] != '"')) {
			cout << "Error on line " << lineNumber << ": The value of properties must be surrounded in double quotes.\n";
		}
		//Must be in properties
		//trim off the '"' from the start/finish of partTwo
		return readProperty(partOne,partTwo.substr(1,partTwo.size()-2));
	}
	return true;
}

//Attempt to read particle data from the file
ParticleData * ParticleData::Parser::ReadParticle(string vpartText) {
	//Reset the state to parse a new file
	lineNumber = 1;
	lastSectionName = "";
	Section = NOSECTION;
	inProgress = new ParticleData();
	//Begin parsing text
	//there are two types of lines
	//header lines "header:"
	//and assignment lines valueName=value
	//the value may span multiple lines if it uses {} or ""
	//but otherwise may not
	//comments begin with # and last until the end of the line
	bool inComment = false;
	//insideChar is \0 normally
	//if inside a value surrounded by qoutes it's a "
	//if inside a value surrounded by {} it's a }
	char insideChar = 0;	

	//Add one more end line to the text to gurantee every line is processed
	vpartText += '\n';

	string partOne;
	string partTwo;
	bool partOneComplete = false;
	for (unsigned int i = 0; i < vpartText.size(); i++) {
		char c = vpartText[i];
		//Keep track of the line number for accurate error messages
		if (c == '\n')
			lineNumber++;

		//When in comment, ignore everything until you reach end of line
		if (inComment) {
			if (c == '\n')
				inComment = false;
		}
		else {
			//Split each line 
			if ((c == '\r') || (c == '\n') || (c == '#') || (c == insideChar)) {
				//Check if there's a surrounding character on this line
				//if so, linebreaks don't indicate the end of the line
				if ((insideChar != '\0') && (c != insideChar))
					continue;
				else if (insideChar != '\0') {
					//The rest of the parser expects the second clause to contain the terminating character
					partTwo += insideChar;
					insideChar = '\0';
				}

				partOne = trimWhitespace(partOne);
				partTwo = trimWhitespace(partTwo);
					

				if (partOneComplete) {
					//Both parts completed, not a header
					//Now parse the line or return NULL on parse failure
					if (!parseLine(partOne,partTwo,false))
						return NULL;
				}
				else if (partOne.size() > 0) {
					//Only one part completed, check for header
					
					if (partOne[partOne.size()-1] != ':') {
						cout << "Error on line " << lineNumber << ": Syntax error, missing '=' or ':'\n";
						return NULL;
					}
					//Parse the header, or return NULL on failure
					if (!parseLine(partOne.substr(0,partOne.size()-1),"",true))
						return NULL;
				}
					
				//Reset buffers
				partOneComplete = false;
				partOne = "";
				partTwo = "";

				//Start of a comment
				if (c == '#')
					inComment = true;
			}
			else if (insideChar != 0) {
				//Convert tabs to spaces
				if (c == '\t')
					c = ' ';
				//Add the character to the buffer
				partTwo += c;
			}
			else if (c == '=') {
				if (partOne.size() <= 0) {
					cout << "Error on line " << lineNumber << ": rule/property is missing name before '='\n";
					return NULL;
				}
				if (partOneComplete) {
					cout << "Error on line " << lineNumber << ": unexpected '=' after initial '='\n";
					return NULL;
				}
				partOneComplete = true;
			}
			//Add characters to correct buffer
			else if (partOneComplete) {
				partTwo += c;
				//If the character starts a property/list, mark that its started
				if ((c == '{') || (c == '"')) {
					if (insideChar != '\0') {
						cout << "Error on line " << lineNumber << ": found block character inside of block. (No nesting blocks)\n";
						return NULL;
					}
					if (c == '{')
						insideChar = '}';
					else if (c == '"')
						insideChar = '"';

				}

			}
			else
				partOne += c;
		}
	}

	return inProgress;
}

ParticleData ParticleData::LoadParticleData(string filename) {
	SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "r"); 
	long size;
	
	//cout << "Parsing particle file \"" << filename << "\":";

	if(!file) {
		//cout << "Failed to open particle data file \"" << filename << "\"";
		return ParticleData();
	}

	//Use the SDL system to read the file
	SDL_RWseek(file , 0 , RW_SEEK_END);
	size = (long)SDL_RWtell(file);
	SDL_RWseek(file,0,RW_SEEK_SET);

	char * fileData = new char[size];
	SDL_RWread(file,fileData, 1, (size_t)size);
	SDL_RWclose(file);

	//Now use an instance of the parser to parse the file
	static Parser parseSystem;
	ParticleData * data = parseSystem.ReadParticle(string(fileData,size));
	//If no error occurred print out that you finished
	if (data == NULL)
		return ParticleData();

	//Hacky, copy to a less temporary storage
	ParticleData temp = *data;
	delete data;

	return temp;
}
