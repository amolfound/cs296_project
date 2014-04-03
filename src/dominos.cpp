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
			//creates chassis shape with polygon
			b2PolygonShape chassis;
			b2Vec2 vertices[8];
			vertices[0].Set(-19.0f, -05.0f);
			vertices[1].Set(19.0f, -05.0f);
			vertices[2].Set(16.5f, 0.0f);
			vertices[3].Set(3.5f, 13.0f);
			vertices[4].Set(-13.0f,13.0f);
			vertices[5].Set(-19.0f, 3.0f);
			chassis.Set(vertices, 6);
			
			//creates circular shape for wheel
			b2CircleShape circle;
			circle.m_radius = 4.0f;
			
			//creates fixture for the chassis with the polygon shape and some density.
			b2FixtureDef fdchassis;
			fdchassis.shape = &chassis;
			fdchassis.density = 5.0f;
			
			//creates the body for chassis in m_world using the above fixture	
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, 10.0f);
			m_car = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_car->CreateFixture(&fdchassis);


			//Wheels
			//creates fixture for wheel
			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 10.0f;
			fd.friction = 0.9f;
			
			//creates rear wheel using fd fixture
			bd.position.Set(-10.0f, 3.5f);
			m_wheel1 = m_world->CreateBody(&bd);
			m_wheel1->CreateFixture(&fd);
			
			//creates front wheel using fd fixture
			bd.position.Set(10.0f, 4.0f);
			m_wheel2 = m_world->CreateBody(&bd);
			m_wheel2->CreateFixture(&fd);
			
			//creates a wheel joint prototype
			b2WheelJointDef jd;
			b2Vec2 axis(0.0f, 1.0f);
			
			//places the joint in the m_world using wheel joint prototype for the rear wheel
			jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 50000.0f;
			jd.enableMotor = true;
			jd.frequencyHz = m_hz;
			jd.dampingRatio = m_zeta;
			m_spring1 = (b2WheelJoint*)m_world->CreateJoint(&jd);
			
			//places the joint in the m_world using wheel joint prototype for the front wheel
			jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 40000.0f;
			jd.enableMotor = false;
			jd.frequencyHz = m_hz;
			jd.dampingRatio = m_zeta;
			m_spring2 = (b2WheelJoint*)m_world->CreateJoint(&jd);
			
			
			{
			//Backside arm attached to chassis
			b2PolygonShape backarm1;
			backarm1.SetAsBox(1,8);
			b2FixtureDef fdbackarm1;
			fdbackarm1.shape = &backarm1;
			fdbackarm1.density = 1.0f;
			
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-20.0f, 10.0f);
			bd.angle = 20* DEGTORAD;
			m_backarm1 = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_backarm1->CreateFixture(&fdbackarm1);
			}
			
			
			{
			//Backsidearm2 attatches to backsidearm
			b2PolygonShape backarm2;
			backarm2.SetAsBox(1,9);
			b2FixtureDef fdbackarm2;
			fdbackarm2.shape = &backarm2;
			fdbackarm2.density = 1.0f;
			
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-30.0f, 10.0f);
			//bd.angle = -20* DEGTORAD;
			m_backarm2 = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_backarm2->CreateFixture(&fdbackarm2);
			}
			 
			{
			//the motorised joint
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_car;
			revoluteJointDef.bodyB = m_backarm1;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(-17.5,-2.7);
			//revoluteJointDef.localAnchorB.Set(-17.5,-2.7);
			revoluteJointDef.localAnchorB.Set(0,-7);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = 15 * DEGTORAD;
			revoluteJointDef.upperAngle =  40 * DEGTORAD;
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 5000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint1 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{
			//the motorised joint
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_backarm1;
			revoluteJointDef.bodyB = m_backarm2;
			//revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(0,7);
			revoluteJointDef.localAnchorB.Set(0,8);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = -(80 * DEGTORAD);
			revoluteJointDef.upperAngle = -(20 * DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 5000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint2 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{
				//Backside pickup
			b2PolygonShape b1;
			b1.SetAsBox(0.3,2);
			b2FixtureDef fdb1;
			fdb1.shape = &b1;
			fdb1.density = 1.0f;
			b2BodyDef bd1;
			bd1.type = b2_dynamicBody;
			bd1.position.Set(-20.0f, 10.0f);
			m_bpick1 = m_world->CreateBody(&bd1);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_bpick1->CreateFixture(&fdb1);
			
			b2PolygonShape b2;
			b2.SetAsBox(2,0.1);
			b2FixtureDef fdb2;
			fdb2.shape = &b2;
			fdb2.density = 1.0f;
			b2BodyDef bd2;
			bd2.type = b2_dynamicBody;
			bd2.position.Set(-20.0f, 10.0f);
			m_bpick2 = m_world->CreateBody(&bd2);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_bpick2->CreateFixture(&fdb2);
			
	
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_bpick1;
			revoluteJointDef.bodyB = m_bpick2;
			revoluteJointDef.localAnchorA.Set(0,-1.5);
			revoluteJointDef.localAnchorB.Set(-1.45,0);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (5 * DEGTORAD);
			revoluteJointDef.upperAngle = (5 * DEGTORAD);
			m_mjoint3 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
     		}
     		
     		{
			//the motorised joint
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_backarm2;
			revoluteJointDef.bodyB = m_bpick1;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(0,-8);
			revoluteJointDef.localAnchorB.Set(0,1.4);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (15 * DEGTORAD);
			revoluteJointDef.upperAngle = (30 * DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 5000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint4 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{
			//Backside arm attatches to chassis
			b2PolygonShape frontarm1;
			frontarm1.SetAsBox(7,0.8);
			b2FixtureDef fdfrontarm1;
			fdfrontarm1.shape = &frontarm1;
			fdfrontarm1.density = 0.80f;
			
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(30.0f, 10.0f);
			//bd.angle = 20* DEGTORAD;
			m_frontarm1 = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_frontarm1->CreateFixture(&fdfrontarm1);
			}
			
			{
			//the motorised joint
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_car;
			revoluteJointDef.bodyB = m_frontarm1;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(17,-2.7);
			revoluteJointDef.localAnchorB.Set(-6,0);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (-30* DEGTORAD);
			revoluteJointDef.upperAngle =  (20* DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 3000.0f;
			m_mjoint5 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
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
		
		case '1':
			m_mjoint1->SetMotorSpeed(1.0f);
			break;	
			
		case '2':
			m_mjoint1->SetMotorSpeed(-1.0f);
			break;
			
		case '3':
			m_mjoint2->SetMotorSpeed(-1.0f);
			break;	
		
		case '4':
			m_mjoint2->SetMotorSpeed(1.0f);
			break;
			
		case '5':
			m_mjoint4->SetMotorSpeed(-1.0f);
			break;
			
		case '6':	
			m_mjoint4->SetMotorSpeed(1.0f);
			break;
			
		case '7':	
			m_mjoint5->SetMotorSpeed(-1.0f);
			break;
			
		case '8':	
			m_mjoint5->SetMotorSpeed(1.0f);
			break;
			
		//~ case '4':
			//~ m_mjoint2->SetMotorSpeed(10.0f);
			//~ break;
		
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
