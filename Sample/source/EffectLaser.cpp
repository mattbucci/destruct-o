#include "stdafx.h"
#include "BaseFrame.h"
#include "EffectLaser.h"
#include "GLEffectProgram.h"
#include "TextureCache.h"


EffectLaser::EffectLaser(vec4 laserColor, float laserWidth) : GLEffectVertexGroup(GL_TRIANGLES,60) {
	state = LASER_IDLE;
	lastUpdated = 0;
	laserPower = 0;
	this->laserColor = laserColor;
	this->laserWidth = laserWidth;
	autoStateChange = false;
	winduptime = 1.0;
	winddowntime = 2.0;
}
EffectLaser::~EffectLaser() {

}

void EffectLaser::SetTiming(float winduptime,float winddowntime, bool autoStateChange) {
	this->winddowntime = winddowntime;
	this->winduptime = winduptime;
	this->autoStateChange = autoStateChange;
}

//Fire-up the laser
void EffectLaser::StartFiring() {
	if (state == LASER_FIRING)
		return;
	state = LASER_WINDINGUP;

}

//Spin down the laser
void EffectLaser::StopFiring() {
	state = LASER_WINDINGDOWN;
}

//Set firing position
void EffectLaser::Move(vec3 startingPos, vec3 endingPos) {
	float anglePart = 180/9.0f;
	if (endingPos == startingPos)
		endingPos+=.05;
	vec3 direction = glm::normalize(endingPos-startingPos);
	vec3 directonAlongPlane;
	//Start with the assumption that
	//the orthogonal vector is: x,1,0
	//solve for x such that (orthogonal vector) dot (direction) = 0
	//x*dir.x+dir.y = 0
	//x = -dir.y/dir.x
	directonAlongPlane = glm::normalize(vec3(-direction.y/direction.x,1.0f,0.0f));



	//Move the laser to those positions
	for (int i = 0; i < 10; i++) {
		int v = i*6;
		float angle = (float)i*anglePart;


		vat(v+0).color = toPOD(vec4(1,1,1,1));
		vat(v+0).textureCoordinate = toPOD(vec2(0,0));
		vat(v+0).vertex = toPOD(vec3(glm::rotate(angle,direction) * vec4(directonAlongPlane*laserWidth,1.0)) + startingPos);
		vat(v+1).color = toPOD(vec4(1,1,1,1));
		vat(v+1).textureCoordinate = toPOD(vec2(1,0));
		vat(v+1).vertex = toPOD(vec3(glm::rotate(angle,direction) * vec4(-directonAlongPlane*laserWidth,1.0)) + startingPos);
		vat(v+2).color = toPOD(vec4(1,1,1,1));
		vat(v+2).textureCoordinate = toPOD(vec2(0,1));
		vat(v+2).vertex = toPOD(vec3(glm::rotate(angle,direction) * vec4(directonAlongPlane*laserWidth,1.0)) + endingPos);
		vat(v+3).color = toPOD(vec4(1,1,1,1));
		vat(v+3).textureCoordinate = toPOD(vec2(1,1));
		vat(v+3).vertex = toPOD(vec3(glm::rotate(angle,direction) * vec4(-directonAlongPlane*laserWidth,1.0)) + endingPos);
		vat(v+4) = vat(v+1);
		vat(v+5) = vat(v+2);
	}
}

//Draw
//call always
//to see laser call StartFiring()
void EffectLaser::Draw(GLEffectProgram * program) {
	if (lastUpdated == 0)
		lastUpdated = Game()->Now();

	double now = Game()->Now();
	double delta =  now-lastUpdated;

	switch (state) {
	case LASER_IDLE:
		break;
	case LASER_WINDINGUP:
		laserPower += delta/winduptime;
		if (laserPower >= 1) {
			laserPower = 1;
			state = LASER_FIRING;
		}
		break;
	case LASER_FIRING:
		if (autoStateChange)
			state = LASER_WINDINGDOWN;
		break;
	case LASER_WINDINGDOWN:
		laserPower -= delta/winddowntime;
		if (laserPower <= 0) {
			laserPower = 0;
			state = LASER_IDLE;
		}
		break;
	}
	lastUpdated = now;
	//Now do the actual drawing
	if (laserPower <= 0)
		return;

	//Reset texture unit 0 to the active texture unit
	glActiveTexture(GL_TEXTURE0);

	//Make pretty laser blending
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	glDisable(GL_CULL_FACE);


	program->UseTexture(true);
	program->SetTint(laserColor*vec4(laserPower,laserPower,laserPower,laserPower));
	Game()->Textures.GetTexture<GLTexture>("effects/laser.png")->Bind();

	GLEffectVertexGroup::Draw(program,true);


	//Return to normal blending
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
