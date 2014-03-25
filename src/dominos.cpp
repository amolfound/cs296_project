/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

/* 
 * Base code for CS 296 Software Systems Lab 
 * Department of Computer Science and Engineering, IIT Bombay
 * Instructor: Parag Chaudhuri
 */


#include "cs296_base.hpp"
#include "render.hpp"

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include "GL/freeglut.h"
#endif

#include <cstring>
using namespace std;

#include "dominos.hpp"

namespace cs296
{
  /**  The is the constructor 
   * This is the documentation block for the constructor.
   */ 
  dominos_t::dominos_t()
    {		
		m_hz = 10.0f;
		m_zeta = 0.7f;
		m_speed = 200.0f;

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = m_world->CreateBody(&bd);

			b2EdgeShape shape;

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 0.0f;
			fd.friction = 0.6f;

			shape.Set(b2Vec2(-200.0f, 0.0f), b2Vec2(200.0f, 0.0f));
			ground->CreateFixture(&fd);

			//float32 hs[10] = {0.25f, 1.0f, 4.0f, 0.0f, 0.0f, -1.0f, -2.0f, -2.0f, -1.25f, 0.0f};

			//float32 x = 20.0f, y1 = 0.0f, dx = 5.0f;
			
		}

		// Car
		{   //chassis
			b2PolygonShape chassis;
			b2Vec2 vertices[8];
			vertices[0].Set(-19.0f, -05.0f);
			vertices[1].Set(19.0f, -05.0f);
			vertices[2].Set(16.5f, 0.0f);
			vertices[3].Set(3.5f, 13.0f);
			vertices[4].Set(-13.0f,13.0f);
			vertices[5].Set(-19.0f, 3.0f);
			chassis.Set(vertices, 6);

			b2CircleShape circle;
			circle.m_radius = 4.0f;
			
			b2FixtureDef fdchassis;
			fdchassis.shape = &chassis;
			fdchassis.density = 5.0f;
				
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, 10.0f);
			m_car = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_car->CreateFixture(&fdchassis);


			//Wheels
			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 10.0f;
			fd.friction = 0.9f;

			bd.position.Set(-10.0f, 3.5f);
			m_wheel1 = m_world->CreateBody(&bd);
			m_wheel1->CreateFixture(&fd);

			bd.position.Set(10.0f, 4.0f);
			m_wheel2 = m_world->CreateBody(&bd);
			m_wheel2->CreateFixture(&fd);

			b2WheelJointDef jd;
			b2Vec2 axis(0.0f, 1.0f);

			jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 50000.0f;
			jd.enableMotor = true;
			jd.frequencyHz = m_hz;
			jd.dampingRatio = m_zeta;
			m_spring1 = (b2WheelJoint*)m_world->CreateJoint(&jd);

			jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 40000.0f;
			jd.enableMotor = false;
			jd.frequencyHz = m_hz;
			jd.dampingRatio = m_zeta;
			m_spring2 = (b2WheelJoint*)m_world->CreateJoint(&jd);
			
			
			{
			//Backside arm attatches to chassis
			b2PolygonShape backarm;
			b2Vec2 vertices[4];
			vertices[0].Set(-18.0f, -04.0f);
			vertices[1].Set(-21.0f, 12.5f);
			vertices[2].Set(-24.0f, 13.0f);
			vertices[3].Set(-16.5, -3.0f);
			backarm.Set(vertices, 4);
			b2FixtureDef fdbackarm;
			fdbackarm.shape = &backarm;
			fdbackarm.density = 1.0f;
			
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, 10.0f);
			m_backarm = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_backarm->CreateFixture(&fdbackarm);
			}
			//~ 
			{
			//the motorised joint
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_car;
			revoluteJointDef.bodyB = m_backarm;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(-17.5,-2.7);
			revoluteJointDef.localAnchorB.Set(-17.5,-2.7);
			revoluteJointDef.enableLimit = true;
			//revoluteJointDef.lowerAngle = -45 * DEGTORAD;
			revoluteJointDef.upperAngle =  20 * DEGTORAD;
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 5000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
		}
	}
    

    void dominos_t::keyboard(unsigned char key)
    {
        switch (key)
		{
		case 'a':
			m_spring1->SetMotorSpeed(m_speed);
			break;

		case 's':
			m_spring1->SetMotorSpeed(0.0f);
			break;

		case 'd':
			m_spring1->SetMotorSpeed(-m_speed);
			break;
		
		case '2':
			m_mjoint->SetMotorSpeed(-10.0f);
			break;
		case '1':
			m_mjoint->SetMotorSpeed(10.0f);
			break;	
		//~ case 'q':
			//~ m_hz = b2Max(0.0f, m_hz - 1.0f);
			//~ m_spring1->SetSpringFrequencyHz(m_hz);
			//~ m_spring2->SetSpringFrequencyHz(m_hz);
			//~ break;
//~ 
		//~ case 'e':
			//~ m_hz += 1.0f;
			//~ m_spring1->SetSpringFrequencyHz(m_hz);
			//~ m_spring2->SetSpringFrequencyHz(m_hz);
			//~ break;
		}
    }

    //~ void dominos_t::keyboardUp(unsigned char key)
    //~ {
        //~ switch (key) {
        //~ case 'q': case 'e': m_circleConveyor.surfaceVelocity = 0; break;
        //~ case 'a': case 'd': m_polygonConveyor.surfaceVelocity = 0; break;
        //~ default: base_sim_t::keyboard(key);
        //~ }
    //~ }
    
  

  sim_t *sim = new sim_t("Dominos", dominos_t::create);
}
