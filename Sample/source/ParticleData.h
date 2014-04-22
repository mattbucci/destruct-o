#pragma once

#include "LinearInterop.h"
#include "InteropChain.h"

#define LinearChain(x) InteropChain<LinearInterop<x>,x>

class ParticleData {
	class Parser {
		//The particle data being parsed currently
		ParticleData * inProgress;
		enum ParserState {
			NOSECTION,
			SYSTEMRULES,
			PARTICLERULES,
			PROPERTIES
		};
		//For debug/errors
		string lastSectionName;
		int lineNumber;
		//The current section of the particle file being parsed
		ParserState Section;

		//Trims all whitespace at the start/end of the in string
		string trimWhitespace(string in);

		//Read a vector of floats into the appropriate type
		//returns false if the vector of floats does not contain enough floats
		//note: 1 float is a special case. 1 float always works because its just used for all components
		bool readIn(vec4 & readInto, vector<float> values);
		bool readIn(vec3 & readInto, vector<float> values);
		bool readIn(vec2 & readInto, vector<float> values);
		bool readIn(float & readInto, vector<float> values);

		//Attempt to read a value (partTwo of a line)
		//reads the value of toRead
		//and places it into readInto
		//if an error occurred returns false
		//else returns true
		template <class T>
		bool readValue(InteropChain<LinearInterop<T>,T> & readInto, string toRead) {
			//First extract the time
			vector<float> readValues;
			string buffer;
			float time;
			bool foundTime = false;
			//Now look through the string
			//first find the time, the float before the ":", then read each value
			//Add a space at the end so when searching for values, you can always assume when you've finished the string
			//you've enumerated all values (there isn't one sitting in the buffer)
			toRead += ' ';
			for (unsigned int i = 0; i < toRead.size(); i++) {
				char c = toRead[i];
				if (foundTime) {
					//At this point all whitespace has been converted to spaces
					if (c == ' ') {
						if (buffer.size() > 0) {
							//must have found a value
							readValues.push_back((float)atof(buffer.c_str()));
							buffer = "";
						}
					}
					else
						buffer += c;
				}
				else {
					//Read until that ":"
					if (c == ':') {
						buffer = trimWhitespace(buffer);
						foundTime = true;
						time = (float)atof(buffer.c_str());
						buffer = "";
					}
					else
						buffer += c;
				}
			}
			//Check that we found some information
			if ((!foundTime) || (readValues.size() <= 0)) {
				cout << "\nError on line " << lineNumber << ": data entry is missing time or data value is missing.\n"; 
				return false;
			}
			//At this point we've got all the information we need
			T value;
			if (!readIn(value,readValues)) {
				cout << "\nError on line " << lineNumber << ": incorrect number of data points for vector data type.\n";
				return false;
			}
			//Finally add your value to the interop chain
			readInto.AddValue(time,value);
			//and we did it!
			return true;
		}

		//determine based off the name and section
		//which interop chain the given value belongs too
		//then use readValue to add the value to the chain
		bool processValue(string valueName, string value);

		//Read in a property
		//all properties are special cases
		//so this handles each property type differently
		//returns false on error
		bool readProperty(string propertyName, string value);

		//Parse a single line of text
		//rules/properties are split into two parts
		//their name, and their contents
		//Headers use partOne as the name of the header, partTwo empty
		//and mark "header" as true
		//returns false on error or true otherwise
		bool parseLine(string partOne, string partTwo, bool header);
	public:

		//Attempt to read a particle from the file
		ParticleData * ReadParticle(string vpartText);
	};
public:
	//Now supports loading from a file
	static ParticleData LoadParticleData(string filename);

	//These properties use the emitter's life factor as the time
	//That is, the input is normalized from 0 to 1, starting at 0
	//and going towards 1 as the emitter gets closer to death
	//if they effect particles they are decided on particle creation
	//The rate which particles are generated
	LinearChain(float) GenerationRate;
	//The initial velocity vector
	LinearChain(vec3) Velocity;
	//X,Y is min, max of random latitude (degrees -90 to 90)
	//applied to starting velocity
	LinearChain(vec2) Latitude;
	//X,Y is min, max of random longitude (degrees -180 to 180)
	//applied to starting velocity
	LinearChain(vec2) Longitude;
	//X,Y is the min/max of the life of the emitted particle
	LinearChain(vec2) Life;
	//The width/length of emitter
	LinearChain(vec2) EmitterSize;
	//The rows/columns of the particle texture
	//note, floor() is applied before rows/columns are used
	float Rows;
	float Columns;
	//The first frame of the texture the particle uses
	//the first value is the min
	//the second value is the max, the used value is floor(random(min,max))
	LinearChain(vec2) FrameOffset;

	//These properties use the particle's life as the time
	//The color of the generated particle
	LinearChain(vec4) Color;
	//The scale of the generated particle
	LinearChain(float) Scale;
	//The acceleration applied to the particle
	//this acceleration is /not/ rotated 
	LinearChain(vec3) Acceleration;
	//The speed at which to play frames from the texture
	//in frames per second
	LinearChain(float) AnimationSpeed;
	//The variation in velocity of generated particles (%)
	//Example: 10 would be 10%, and any Velocity value could be up to 10% higher or lower
	LinearChain(float) VelocityVariation;
	//The variation in scale of the generated particles (%)
	LinearChain(float) ScaleVariation;

	//Properties for the material
	enum MaterialEffect {
		//No blending (like voxels)
		NONE,
		//Transparency is allowed
		//sorting is required
		BLEND,
		//Color is added to already drawn colors
		ADDITIVE,
		//Similar to additive, but reverses color first
		SCREEN,
	};
	MaterialEffect MaterialStyle;
	//Should be the texture name relative to the particle texture directory
	string MaterialTexture;
}; 