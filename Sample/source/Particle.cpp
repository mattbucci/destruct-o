#include "stdafx.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "OS.h"
#include "ParticleData.h"

//Note this should be replaced probably
float random(float min, float max) {
	return ((float)rand()/(float)(RAND_MAX))*(max-min)+min;
}

Particle::Particle(double gameTime, float systemTime, ParticleSystem * owner, ParticleData * systemData) {
	this->owner = owner;
	this->systemData = systemData;
	UpdateGroup(GL_TRIANGLE_STRIP,4);
	//Build the basic particle
	static const float testSize = .5;
	vat(0) = vec3(testSize,testSize,0);
	xat(0) = vec2(1,1);
	vat(1) = vec3(-testSize,testSize,0);
	xat(1) = vec2(0,1);
	vat(2) = vec3(testSize,-testSize,0);
	xat(2) = vec2(1,0);
	vat(3) = vec3(-testSize,-testSize,0);
	xat(3) = vec2(0,0);

	//Now assign system-based properties
	velocity = systemData->Velocity.ValueAtSequence(systemTime);
	velocity += velocity*(random(-1,1)*systemData->Variation.ValueAtSequence(systemTime));
	deathAt = gameTime + systemData->Life.ValueAtSequence(systemTime);
	//Choose a position in the emitter
	position = owner->Position;

	vec2 emitterSize = systemData->EmitterSize.ValueAtSequence(systemTime);
	position += vec3(random(emitterSize.x*-.5f,emitterSize.x*.5f),random(emitterSize.y*-.5f,emitterSize.y*.5f),0);

	//Set all the normals to face the camera
	for (int i = 0; i < 4; i++)
		nat(i) = vec3(0,0,1);
	//Set your spawn time to right now
	spawn = gameTime;
}

//Draw this single particle. Must be called form the global context
void Particle::Draw(const mat4 & view, GL3DProgram * shaders) {
	/*vec4 color(1,1,1,visibility);

	//Don't even bother to draw if visibility is this low
	if (visibility <= .001)
		return;

	//Calculate the draw properties as of right now
	//First calculate segment information
	float life = (float)(time-spawn);
	vec4 col = owner->color.ValueAtSequence(life);
	color *= col;
	float scale = owner->scale.ValueAtSequence(life);

	//Now apply the layer
	//Shading
	if (parent->flags & MdxNode::FlagUnshaded)
		shaders->Lights.Off();
	else
		shaders->Lights.On();

	//Apply the lighting change
	shaders->Lights.Apply();

	//Apply the cull face flag
	glDisable(GL_CULL_FACE);

	//Blend mode
	//These were discovered through some trial and error
	//and some referencing the directX version and changing to opengl alternatives
	switch(parent->filter) {
	case MdxParticleEmitter2::None:
		glEnable (GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case MdxParticleEmitter2::Additive:
		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		break;
	case MdxParticleEmitter2::AlphaKey:
		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case MdxParticleEmitter2::Modulate2x:
	case MdxParticleEmitter2::Modulate:
		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		//glBlendFunc(GL_DST_COLOR,GL_ZERO);
		//glDisable(GL_BLEND);
		break;
	}
	glDepthMask(GL_FALSE);

	//FOR TESTING BEGIN
	//glDisable(GL_BLEND);
	//glDisable(GL_ALPHA_TEST);
	//FOR TESTING END
	 

	int texId = (int)parent->textureId;
	
	
	GLMaterial geosetTexture;
	geosetTexture.SetShininess(0,1);
	geosetTexture.SetColor(color);
	if (texId >= 0) {
		//Lookup the texture used by this particle
		//in the model's texture list
		Texture & texture = owner->parent->textures[texId];
		int rep = texture.replaceableId;
		if (texture.replaceableId == 1)
			geosetTexture.SetColor(vec4(teamcolor,color.a));
		else {
			GLTexture * glTexture = System::Map->Textures.GetTexture("w3\\" + texture.texturePath);
			geosetTexture.SetTexture(glTexture);
		}
	}
	//Apply the material 
	shaders->Materials.ApplyMaterial(&geosetTexture);

	//Calculate the current frame and the texture coordinates
	//that go with that frame
	int frameLengths[4];
	int totalFrames = 0;

	//Figure out which sections to use
	int start;
	if ((parent->headOrTail == MdxParticleEmitter2::BOTH) || (parent->headOrTail == MdxParticleEmitter2::HEAD))
		start = 0;
	else
		start = 2;

	int end;
	if ((parent->headOrTail == MdxParticleEmitter2::BOTH) || (parent->headOrTail == MdxParticleEmitter2::TAIL))
		end = 4;
	else
		end = 2;

	
	if (MdxParticleEmitter2::Modulate == parent->Modulate && life > .25)
		cout << "";

	for (int i = start; i < end; i++) {
		frameLengths[i] = (int)((int)(parent->intervals[i][1] - parent->intervals[i][0] + 1) * parent->intervals[i][2]);
		totalFrames += frameLengths[i];
	}


	int currentFrame = ((float)totalFrames * life / parent->lifespan);
	int frameCheck = 0;
	int index = 0;
	for (int i = start; i < end; i++) {
		frameCheck += frameLengths[i];
		if (currentFrame < frameCheck) {
			int frameStart = parent->intervals[i][0];
			int frameEnd = parent->intervals[i][1];
			int frameInterval = frameEnd - frameStart + 1;
			int frameIntervalStart = frameCheck-frameLengths[i];
			index = frameStart + ((currentFrame - frameIntervalStart) % frameInterval);
			break;
		}
	}


	//Now use the correctly calculated index to determine the currently
	//visible frame of the texture
	if (parent->columns == 0) {
		xat(0) = vec2(1,1);
		xat(1) = vec2(0,1);
		xat(2) = vec2(1,0);
		xat(3) = vec2(0,0);
	}
	else {
		float cols = parent->columns;
		float rows = parent->rows;
		int row = index/parent->columns;
		int col = index % parent->columns;
		vec2 a,b,c,d;
		a = xat(0) = vec2((col+1)/cols,(row+1)/rows);
		b = xat(1) = vec2(col/cols,(row+1)/rows);
		c = xat(2) = vec2((col+1)/cols,row/rows);
		d = xat(3) = vec2(col/cols,row/rows);
		//for (int i = 0; i < 4; i++)
			//xat(i) = vec2(1-xat(i).x,xat(i).y);
	}*/

	//Setup the model matrix
	shaders->Model.PushMatrix();
	shaders->Model.Translate(position);
	shaders->Model.Scale(scale);
	shaders->Model.Rotate((position.x+position.y+position.z),velocity.x+velocity.y,velocity.x+position.z,velocity.z);
	//shaders->Model.Billboard(view,vec3());
	//shaders->Model.Billboard(shaders->Camera.GetZAxis());
	shaders->Model.Apply();

	//use an ugly billboarding approach

	//shaders->SetUniform2f(vec2(1,0),"billboardhack");
	//Render the particle
	GLDynamicVertexGroup::Draw(shaders);

	//shaders->SetUniform2f(vec2(0,0),"billboardhack");

	//Undo the matrix
	shaders->Model.PopMatrix();
}
//Updates the position of the particle
bool Particle::Update(double time, double delta) {
	if (time >= deathAt)
		//Mark this particle for destruction
		return false;


	//Calculate acceleration
	float life = (float)(time-spawn);
	float lifeScale = life/(float)(deathAt-spawn);

	//Use the midpoint method update formula to choose the new velocity/position
	//First calculation
	vec3 acceleration1,acceleration2;
	acceleration1 = systemData->Acceleration.ValueAtSequence(life);
	if (systemData->ForceFunction)
		acceleration1 += systemData->ForceFunction(lifeScale,position,velocity);
	//apply the calculated force with half the delta
	velocity += acceleration1*(float)delta/2.0f;
	position += velocity*(float)delta/2.0f;
	//Now recalculate the applied force at this new position
	acceleration2 = systemData->Acceleration.ValueAtSequence(life);
	if (systemData->ForceFunction)
		acceleration2 += systemData->ForceFunction(lifeScale,position,velocity);
	//apply the final calculated force with the full data
	velocity += acceleration2*(float)delta;
	position += velocity*(float)delta;

	//Other properties needed for draw
	color = systemData->Color.ValueAtSequence(lifeScale);
	scale = systemData->Scale.ValueAtSequence(lifeScale);
	//This particle lives a little longer
	return true;
}