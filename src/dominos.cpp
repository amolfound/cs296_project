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
		{   
			{//chassis
			//creates chassis shape with polygon
			b2PolygonShape chassis;
			b2Vec2 vertices[5];
			vertices[0].Set(-19.0f, -05.0f);
			vertices[1].Set(19.0f, -05.0f);
			vertices[2].Set(19.0f, 5.0f);
			vertices[3].Set(0.0f, 8.0f);
			vertices[4].Set(-19.0f, 8.0f);
			chassis.Set(vertices, 5);

			
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
			
			b2PolygonShape chassis_up;
			chassis_up.SetAsBox(9.5,7);
			fdchassis.shape = &chassis_up;
			bd.position.Set(-9.5f,25.0f);
			m_car_up = m_world->CreateBody(&bd);
			m_car_up->CreateFixture(&fdchassis);
			
			b2WeldJointDef wj;
			wj.Initialize(m_car, m_car_up, m_car_up->GetPosition());
			m_world->CreateJoint(&wj);
			}
			
			{
			//creates circular shape for wheel
			b2CircleShape circle;
			circle.m_radius = 7.0f;
			
			//Wheels
			//creates fixture for wheel
			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 10.0f;
			fd.friction = 0.9f;
			fd.filter.groupIndex=-1;
			
			//creates rear wheel using fd fixture
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-14.0f, 6.5f);
			m_wheel1 = m_world->CreateBody(&bd);
			m_wheel1->CreateFixture(&fd);
			
			//creates front wheel using fd fixture
			circle.m_radius = 6.0f;
			bd.position.Set(13.0f, 6.0f);
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
			}
			
			{
			//Backside arm attached to chassis
			b2PolygonShape backarm1;
			backarm1.SetAsBox(1,8);
			b2FixtureDef fdbackarm1;
			fdbackarm1.shape = &backarm1;
			fdbackarm1.density = 1.0f;
			fdbackarm1.filter.groupIndex = -1;
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
			//the  joint joins backside arm 1 with chassis
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
			revoluteJointDef.maxMotorTorque = 7000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint1 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{
			//the  joint joins backside arm 1 with backside arm 2 
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
			fdb2.density = 2.0f;
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
			revoluteJointDef.lowerAngle = (0 * DEGTORAD);
			revoluteJointDef.upperAngle = (30 * DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 5000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint4 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{
			//Frontarm1 arm attatches to chassis
			b2PolygonShape frontarm1;
			frontarm1.SetAsBox(7,0.7);
			b2FixtureDef fdfrontarm1;
			fdfrontarm1.filter.groupIndex = -1;
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
			revoluteJointDef.lowerAngle = (45* DEGTORAD);
			revoluteJointDef.upperAngle =  (70* DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 3000.0f;
			m_mjoint5 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{
			//Frontarm2 arm attatches to frontarm1
			b2PolygonShape frontarm2;
			frontarm2.SetAsBox(6,0.6);
			b2FixtureDef fdfrontarm2;
			fdfrontarm2.shape = &frontarm2;
			fdfrontarm2.density = 0.50f;
			fdfrontarm2.filter.groupIndex=-1;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(40.0f, 10.0f);
			m_frontarm2 = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_frontarm2->CreateFixture(&fdfrontarm2);
			}
			
			{
			//the motorised joint
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_frontarm1;
			revoluteJointDef.bodyB = m_frontarm2;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(6,-0);
			revoluteJointDef.localAnchorB.Set(-5,0);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (-130* DEGTORAD);
			revoluteJointDef.upperAngle =  (-110* DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 5000.0f;
			m_mjoint6 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{//piston support joins to frontarm2
			b2PolygonShape pistonsupp;
			pistonsupp.SetAsBox(1.9,0.2);
			b2FixtureDef fdpistonsupp;
			fdpistonsupp.filter.groupIndex=-1;
			fdpistonsupp.shape = &pistonsupp;
			fdpistonsupp.density = 0.1f;
			fdpistonsupp.friction= 0.5f;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			m_pistonsupp = m_world->CreateBody(&bd);
			m_pistonsupp->CreateFixture(&fdpistonsupp);
			
			//joint to join piston to frontarm2 
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_frontarm2;
			revoluteJointDef.bodyB = m_pistonsupp;
			revoluteJointDef.localAnchorA.Set(5.5,0);
			revoluteJointDef.localAnchorB.Set(1.5,0);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (180 * DEGTORAD);
			revoluteJointDef.upperAngle = (180 * DEGTORAD);
			m_mjoint7 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );	
			}	
			
			{
			b2PolygonShape piston1;
			piston1.SetAsBox(1,0.2);
			b2FixtureDef fdpiston1;
			fdpiston1.shape = &piston1;
			fdpiston1.density = 0.001f;
			fdpiston1.filter.groupIndex=-1;
			b2BodyDef bd;
			bd.position.Set(40.0f, 10.0f);
			bd.type = b2_dynamicBody;
			m_piston1 = m_world->CreateBody(&bd);
			m_piston1->CreateFixture(&fdpiston1);
			
			b2PolygonShape piston2;
			piston2.SetAsBox(1,0.2);
			b2FixtureDef fdpiston2;
			fdpiston2.shape = &piston2;
			fdpiston2.density = 0.001f;
			fdpiston2.filter.groupIndex=-1;
			b2BodyDef bd2;
			bd2.position.Set(41.0f, 10.0f);
			bd2.type = b2_dynamicBody;
			m_piston2 = m_world->CreateBody(&bd2);
			m_piston2->CreateFixture(&fdpiston2);
			 
			b2PolygonShape piston3;
			piston3.SetAsBox(0.2,1.0);
			b2FixtureDef fdpiston3;
			fdpiston3.shape = &piston3;
			fdpiston3.density = 0.001f;
			fdpiston3.friction= 0.5f;
			fdpiston3.filter.groupIndex=-1;
			b2BodyDef bd3;
			bd3.position.Set(44.0f, 10.0f);
			bd3.type = b2_dynamicBody;
			m_piston3 = m_world->CreateBody(&bd3);
			m_piston3->CreateFixture(&fdpiston3);
			
			 
			//joint to join piston1 to pistonsupp 
			b2RevoluteJointDef revoluteJointDef2;
			revoluteJointDef2.bodyA = m_pistonsupp;
			revoluteJointDef2.bodyB = m_piston1;
			revoluteJointDef2.collideConnected = false;
			revoluteJointDef2.localAnchorA.Set(0,0);
			revoluteJointDef2.localAnchorB.Set(-1,0);
			revoluteJointDef2.enableLimit = true;
			revoluteJointDef2.lowerAngle = (-90 * DEGTORAD);
			revoluteJointDef2.upperAngle = (-90 * DEGTORAD);
			m_mjoint8 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef2 );	
			
			//joint to join piston2 to piston3
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_piston2;
			revoluteJointDef.bodyB = m_piston3;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(0.9,0);
			revoluteJointDef.localAnchorB.Set(0,0.9);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (0 * DEGTORAD);
			revoluteJointDef.upperAngle = (0 * DEGTORAD);
			m_mjoint9 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );	
			
			//prismatic joint for piston creation
			b2PrismaticJointDef pjd;
			pjd.bodyA = m_piston1;
			pjd.bodyB = m_piston2;
			pjd.collideConnected = false;
			pjd.localAxisA.Set(1,0);
			pjd.localAnchorA.Set( 0.8f,0);
			pjd.localAnchorB.Set(-0.4f,0);
			pjd.enableMotor = true;
			pjd.maxMotorForce = 100.0f;
			pjd.enableLimit = true;
			pjd.lowerTranslation = 0.12f;
			pjd.upperTranslation = 0.6f;
			m_pistonjoint = (b2PrismaticJoint*)m_world->CreateJoint(&pjd);		
			}
			
			//testbox
			{
			b2PolygonShape testbox;
			testbox.SetAsBox(1.4,1.4);
			b2FixtureDef fdtestbox;
			fdtestbox.shape = &testbox;
			fdtestbox.density = 0.0001f;
			fdtestbox.friction=0.5;
			b2BodyDef bd;
			bd.position.Set(40.0f, 1.0f);
			bd.type = b2_dynamicBody;
			test_box1 = m_world->CreateBody(&bd);
			test_box1->CreateFixture(&fdtestbox);
			}
			
			//testball
			{
			b2CircleShape circle;
			circle.m_radius = 1.1f;
			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 1.5f;
			fd.friction = 0.9f;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-35.0f, 3.5f);
			test_ball = m_world->CreateBody(&bd);
			test_ball->CreateFixture(&fd);
			}
			
			//fixed obstacle
			{
			b2PolygonShape testbox;
			testbox.SetAsBox(0.1,1);
			b2FixtureDef fdtestbox;
			fdtestbox.shape = &testbox;
			b2BodyDef bd;
			bd.position.Set(-33.0f, 1);
			obs = m_world->CreateBody(&bd);
			obs->CreateFixture(&fdtestbox);
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
			m_mjoint1->SetMotorSpeed(0.2f);
			break;	
			
		case '2':
			m_mjoint1->SetMotorSpeed(-1.0f);
			break;
			
		case '3':
			m_mjoint2->SetMotorSpeed(-1.0f);
			break;	
		
		case '4':
			m_mjoint2->SetMotorSpeed(0.3f);
			break;
			
		case '5':
			m_mjoint4->SetMotorSpeed(-1.0f);
			break;
			
		case '6':	
			m_mjoint4->SetMotorSpeed(1.0f);
			break;
			
		case '7':	
			m_mjoint5->SetMotorSpeed(1.0f);
			break;
			
		case '8':	
			m_mjoint5->SetMotorSpeed(-1.0f);
			break;
		
		case '9':	
			m_mjoint6->SetMotorSpeed(-1.0f);
			break;
		
		case '0':	
			m_mjoint6->SetMotorSpeed(1.0f);
			break;	
			
		case 'u':	
			m_pistonjoint->SetMotorSpeed(-2.0f);
			break;
			
		case 'i':	
			m_pistonjoint->SetMotorSpeed(2.0f);
			break;
		}
    }

  

  sim_t *sim = new sim_t("Dominos", dominos_t::create);
}
