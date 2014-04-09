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
#include <math.h>
#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include "GL/freeglut.h"
#endif

#include <cstring>
using namespace std;
#include "callbacks.hpp"
#include "dominos.hpp"

namespace cs296
{
  /**  The is the constructor 
   * This is the documentation block for the constructor.
   */ 
  dominos_t::dominos_t()
    {		
		m_hz = 10.0f;
		m_zeta = 0.2f;
		m_speed = 300.0f;
		int num=360;
		int count=0;
		float y;
		b2Vec2 groundVertices[num];
		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			bd.position.Set(0.0f,0.0f);
			bd.type=b2_staticBody;
			ground = m_world->CreateBody(&bd);
			int w=0;
			b2ChainShape groundShape;
			y=-20;
			float y1;
			
			for(int j=0;j<7;j++) {
				for(int i=0;i<45;i++) {
				
					y1=rand() % 2 - 1;
					if(w==1) groundVertices[j*45 +i].Set(3*(j*45.0f +i)-100.0f , y - y1);
					else if(w==2) groundVertices[j*45 +i].Set(3*(j*45.0f +i)-100.0f , y + y1);
					else groundVertices[j*45 +i].Set(3*(j*45.0f +i)-100.0f , y);
					y=groundVertices[j*45 +i].y;	
					count+=1;
				}
				w=rand() % 3  ;
			}
			
			groundShape.CreateChain(groundVertices,count);
			b2FixtureDef fd;
			fd.shape = &groundShape;
			fd.density = 0.0f;
			fd.friction = 0.6f;
			
			ground->CreateFixture(&fd);
		}
		
		{
			b2PolygonShape bigbin1;
			b2PolygonShape bigbin2;
			b2PolygonShape bigbin3;
			
			b2Vec2 vertices1[4];
			b2Vec2 vertices2[4];
			b2Vec2 vertices3[4];
			vertices1[0].Set(0.0f, 0.0f);
			vertices1[3].Set(0.0f, -100.0f);
			vertices1[2].Set(10.0f, -100.0f);
			vertices1[1].Set(10.0f, 0.0f);
			bigbin1.Set(vertices1, 4);
			vertices2[0].Set(10.0f, -100.0f);
			vertices2[1].Set(10.0f, -90.0f);
			vertices2[2].Set(100.0f,-90.0f);
			vertices2[3].Set(100.0f, -100.0f);
			bigbin2.Set(vertices2, 4);
			vertices3[0].Set(100.0f, -100.0f);
			vertices3[1].Set(100.0f, 0.0f);
			vertices3[2].Set(110.0f, 0.0f);
			vertices3[3].Set(110.0f, -100.0f);
			bigbin3.Set(vertices3, 4);

			
			//creates fixture for the chassis with the polygon shape and some density.
			//b2FixtureDef fdbin;
			//fdbin.shape = &Bin;
			//fdbin.density = 0.0f;
			//creates the body for chassis in m_world using the above fixture	
			b2BodyDef bd2;
			bd2.type = b2_staticBody;
			bd2.position.Set(844.0f, y);
			m_bigbin = m_world->CreateBody(&bd2);
			m_bigbin->CreateFixture(&bigbin1, 1.0f);
			m_bigbin->CreateFixture(&bigbin2, 1.0f);
			m_bigbin->CreateFixture(&bigbin3, 1.0f);
		}
		{
			b2PolygonShape bin1;
			b2PolygonShape bin2;
			b2PolygonShape bin3;
			
			b2Vec2 vertices1[4];
			b2Vec2 vertices2[4];
			b2Vec2 vertices3[4];
			vertices1[0].Set(0.0f, 0.0f);
			vertices1[1].Set(0.0f, 14.0f);
			vertices1[2].Set(2.0f, 14.0f);
			vertices1[3].Set(2.0f, 0.0f);
			bin1.Set(vertices1, 4);
			vertices2[0].Set(2.0f, 0.0f);
			vertices2[1].Set(2.0f, 2.0f);
			vertices2[2].Set(12.0f,2.0f);
			vertices2[3].Set(12.0f, 0.0f);
			bin2.Set(vertices2, 4);
			vertices3[0].Set(12.0f, 0.0f);
			vertices3[1].Set(12.0f, 14.0f);
			vertices3[2].Set(14.0f, 14.0f);
			vertices3[3].Set(14.0f, 0.0f);
			bin3.Set(vertices3, 4);

			
			//creates fixture for the chassis with the polygon shape and some density.
			//b2FixtureDef fdbin;
			//fdbin.shape = &Bin;
			//fdbin.density = 0.0f;
			//creates the body for chassis in m_world using the above fixture	
			b2BodyDef bd;
			bd.type = b2_staticBody;
			bd.position.Set(-100.0f, -20.0f);
			m_bin = m_world->CreateBody(&bd);
			m_bin->CreateFixture(&bin1, 1.0f);
			m_bin->CreateFixture(&bin2, 1.0f);
			m_bin->CreateFixture(&bin3, 1.0f);
			
			
		}
		
		

		
		{	// Car  
			{	//chassis
			
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
			fdchassis.filter.groupIndex =-1;
			//creates the body for chassis in m_world using the above fixture	
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, -10.0f);
			m_car = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_car->CreateFixture(&fdchassis);
			
			b2PolygonShape chassis_up;
			chassis_up.SetAsBox(9.5,7);
			fdchassis.shape = &chassis_up;
			fdchassis.filter.groupIndex = -1;
			bd.position.Set(-9.5f,5.0f);
			m_car_up = m_world->CreateBody(&bd);
			m_car_up->CreateFixture(&fdchassis);
			
			b2WeldJointDef wj;
			wj.Initialize(m_car, m_car_up, m_car_up->GetPosition());
			m_world->CreateJoint(&wj);
			
			b2PolygonShape hinge;
			hinge.SetAsBox(2,2);
			fdchassis.shape = &hinge;
			bd.position.Set(-21.0f,-13.0f);
			m_back_hinge = m_world->CreateBody(&bd);
			m_back_hinge->CreateFixture(&fdchassis);
			
			wj.Initialize(m_car, m_back_hinge, m_back_hinge->GetPosition());
			m_world->CreateJoint(&wj);
			}
			
			{ //gas pipe
			b2PolygonShape pipe;
			b2Vec2 vertices[5];
			vertices[0].Set(-1.2f, 0.0f);
			vertices[1].Set(1.2f, 0.0f);
			vertices[2].Set(1.0f, 1.0f);
			vertices[3].Set(-1.0f, 1.0f);
			pipe.Set(vertices, 4);

			b2FixtureDef fdpipe;
			fdpipe.shape = &pipe;
			fdpipe.filter.groupIndex = -1;
			fdpipe.density = 0.5f;
				
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(10.0f, -3.8f);
			m_pipe_part1 = m_world->CreateBody(&bd);
			m_pipe_part1->CreateFixture(&fdpipe);
			
			b2WeldJointDef wj;
			wj.Initialize(m_car, m_pipe_part1, m_pipe_part1->GetPosition());
			m_world->CreateJoint(&wj);
			
			vertices[0].Set(1.0f, 1.0f);
			vertices[1].Set(1.0f, 7.0f);
			vertices[2].Set(0.0f, 9.0f);
			vertices[3].Set(-1.0f, 7.0f);
			vertices[4].Set(-1.0f, 1.0f);
			pipe.Set(vertices, 5);
			
			m_pipe_part2 = m_world->CreateBody(&bd);
			m_pipe_part2->CreateFixture(&fdpipe);
			
			wj.Initialize(m_pipe_part1, m_pipe_part2, m_pipe_part2->GetPosition());
			m_world->CreateJoint(&wj);
			
			vertices[0].Set(1.0f, 7.0f);
			vertices[1].Set(1.5f, 7.2f);
			vertices[2].Set(1.5f, 9.2f);
			vertices[3].Set(0.0f, 9.0f);
			pipe.Set(vertices, 4);
			
			m_pipe_part2->CreateFixture(&fdpipe);
			
						
			}
			
			{	//Wheels
			//creates circular shape for wheel
			b2CircleShape circle;
			circle.m_radius = 7.0f;
			
			//creates fixture for wheel
			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 5.0f;
			fd.friction = 0.9f;
			fd.restitution = 0.0f;
			fd.filter.groupIndex=-1;
			
			//creates rear wheel using fd fixture
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-14.0f, -13.5f);
			m_wheel1 = m_world->CreateBody(&bd);
			m_wheel1->CreateFixture(&fd);
			
			//creates front wheel using fd fixture
			circle.m_radius = 6.0f;
			bd.position.Set(13.0f, -14.0f);
			m_wheel2 = m_world->CreateBody(&bd);
			m_wheel2->CreateFixture(&fd);
			
			//creates a wheel joint prototype
			b2WheelJointDef jd;
			b2Vec2 axis(0.0f, 1.0f);
			
			//places the joint in the m_world using wheel joint prototype for the rear wheel
			jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 150000.0f;
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
			
			{	//Backside arm attached to back_hinge
			
			b2PolygonShape backarm1;
			b2Vec2 vertices[5];
			vertices[0].Set(0.0f, -10.0f);
			vertices[1].Set(2.0f, -10.0f);
			vertices[2].Set(2.0f, 0.0f);
			vertices[3].Set(-4.0f, 16.0f);
			vertices[4].Set(-5.0f, 16.0f);
			backarm1.Set(vertices,5);
			
			b2FixtureDef fdbackarm1;
			fdbackarm1.shape = &backarm1;
			fdbackarm1.density = 1.0f;
			fdbackarm1.filter.groupIndex = -1;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-22.0f, -5.0f);
			m_backarm1 = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_backarm1->CreateFixture(&fdbackarm1);
			}
			
			{	//Backsidearm2 attached to backsidearm
			b2PolygonShape backarm2;
			b2Vec2 vertices[6];
			vertices[0].Set(-0.5f, -14.0f);
			vertices[1].Set(-2.0f, 13.5f);
			vertices[2].Set(-1.33f, 14.0f);
			vertices[3].Set(1.33f, 14.0f);
			vertices[4].Set(2.0f, 13.5f);
			vertices[5].Set(0.5f, -14.0f);
			backarm2.Set(vertices,6);
			b2FixtureDef fdbackarm2;
			fdbackarm2.shape = &backarm2;
			fdbackarm2.density = 1.0f;
			
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-28.0f, -3.0f);
			m_backarm2 = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_backarm2->CreateFixture(&fdbackarm2);
			}
			 
			{	//the  joint joins backside arm 1 with chassis
			
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_back_hinge;
			revoluteJointDef.bodyB = m_backarm1;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(0,0);
			revoluteJointDef.localAnchorB.Set(1,-9);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = -20 * DEGTORAD;
			revoluteJointDef.upperAngle =  30 * DEGTORAD;
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 18000.0f;
			m_mjoint1 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{	//the  joint joins backside arm 1 with backside arm 2 
			
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_backarm1;
			revoluteJointDef.bodyB = m_backarm2;
			//revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(-4.5,16);
			revoluteJointDef.localAnchorB.Set(0,13.0);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = -(80 * DEGTORAD);
			revoluteJointDef.upperAngle = -(20 * DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 12000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint2 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{	//Backside pickup
			
			b2PolygonShape b1;
			b2Vec2 vertices[4];
			vertices[0].Set(0.0f, 3.0f);
			vertices[1].Set(-1.0f, 1.0f);
			vertices[2].Set(-1.5f, 2.0f);
			b1.Set(vertices, 3);
			
			b2FixtureDef fdb1;
			fdb1.shape = &b1;
			fdb1.density = 1.0f;
			fdb1.friction = 0;
			b2BodyDef bd1;
			bd1.type = b2_dynamicBody;
			bd1.position.Set(-25.0f, -17.0f);
			bd1.angle = 50* DEGTORAD;
			m_bpick = m_world->CreateBody(&bd1);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_bpick->CreateFixture(&fdb1);
			
			vertices[0].Set(-1.5f, 0.2f);
			vertices[1].Set(-1.0f, 1.0f);
			vertices[2].Set(-1.5f, 2.0f);
			vertices[3].Set(-2.0f, 0.0f);
			b1.Set(vertices, 4);
			fdb1.shape = &b1;
			m_bpick->CreateFixture(&fdb1);
			
			
			vertices[0].Set(-1.5f, 0.2f);
			vertices[1].Set(-1.0f, -2.0f);
			vertices[2].Set(-1.5f, -2.0f);
			vertices[3].Set(-2.0f, 0.0f);
			b1.Set(vertices, 4);
			fdb1.shape = &b1;
			m_bpick->CreateFixture(&fdb1);
			
			vertices[0].Set(-1.0f, -2.0f);
			vertices[1].Set(-1.5f, -2.0f);
			vertices[2].Set(1.0f, -3.0f);
			b1.Set(vertices, 3);
			fdb1.shape = &b1;
			m_bpick->CreateFixture(&fdb1);
     		}
     		
     		{	//the motorised joint of backside pickup
     		
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_backarm2;
			revoluteJointDef.bodyB = m_bpick;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(0,-13.5);
			revoluteJointDef.localAnchorB.Set(0,2.9);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (30 * DEGTORAD);
			revoluteJointDef.upperAngle = (90 * DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 7000.0f;
			//revoluteJointDef.motorSpeed = -90 * DEGTORAD;//90 degrees per second
			m_mjoint4 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{	//Frontarm1 arm attatched to chassis
			
			b2PolygonShape frontarm1;
			frontarm1.SetAsBox(14,1);
			b2FixtureDef fdfrontarm1;
			fdfrontarm1.filter.groupIndex = -1;
			fdfrontarm1.shape = &frontarm1;
			fdfrontarm1.density = 1.0f;
			
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(14.0f, -5.0f);
			//bd.angle = 20* DEGTORAD;
			m_frontarm1 = m_world->CreateBody(&bd);
			//m_car->CreateFixture(&chassis, 1.0f);
			m_frontarm1->CreateFixture(&fdfrontarm1);
			m_frontarm1->SetGravityScale(0.1f);
			}
			
			{	//the motorised joint between frontarm1 and chassis
			
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_car;
			revoluteJointDef.bodyB = m_frontarm1;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(0,7);
			revoluteJointDef.localAnchorB.Set(-14,0);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (-30* DEGTORAD);
			revoluteJointDef.upperAngle =  (0* DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 7000.0f;
			m_mjoint5 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{	//Digger attached to the frontarm1
			
			b2PolygonShape digger;
			b2Vec2 vertices[3];
			vertices[0].Set(0.0f,5.0f);
			vertices[1].Set(-2.0f,-6.0f);
			vertices[2].Set(2.0f,-4.0f);
			digger.Set(vertices,3);
			b2FixtureDef fddigger;
			fddigger.shape = &digger;
			fddigger.density = 1.0f;
			
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(28.0f, -5.0f);
			//bd.angle = 20* DEGTORAD;
			m_digger = m_world->CreateBody(&bd);
			m_digger->SetGravityScale(0.1f);
			
			//m_car->CreateFixture(&chassis, 1.0f);
			m_digger->CreateFixture(&fddigger);
			
			vertices[0].Set(-2.0f,-6.0f);
			vertices[1].Set(11.0f,-5.0f);
			vertices[2].Set(2.0f,-4.0f);
			digger.Set(vertices,3);
			m_digger->CreateFixture(&fddigger);
			
			}
			
			
			{	//the revolute joint between frontarm1 and digger
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_frontarm1;
			revoluteJointDef.bodyB = m_digger;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(14,0);
			revoluteJointDef.localAnchorB.Set(0,-1);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (10* DEGTORAD);
			revoluteJointDef.upperAngle =  (50* DEGTORAD);
			revoluteJointDef.enableMotor = true;
			revoluteJointDef.maxMotorTorque = 7000.0f;
			m_mjoint6 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			{	//the piston for frontarm
			b2PolygonShape piston;
			piston.SetAsBox(3,1);
			b2FixtureDef fdpiston;
			fdpiston.filter.groupIndex = -1;
			fdpiston.shape = &piston;
			fdpiston.density = 2.0f;
			fdpiston.friction = 0;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(3.0f, -10.0f);
			bd.angle = 20* DEGTORAD;
			m_piston1 = m_world->CreateBody(&bd);
			m_piston1->CreateFixture(&fdpiston);
			
			b2RevoluteJointDef revoluteJointDef;
			revoluteJointDef.bodyA = m_car;
			revoluteJointDef.bodyB = m_piston1;
			revoluteJointDef.collideConnected = false;
			revoluteJointDef.localAnchorA.Set(0,0);
			revoluteJointDef.localAnchorB.Set(-3,0);
			revoluteJointDef.enableLimit = true;
			revoluteJointDef.lowerAngle = (10* DEGTORAD);
			revoluteJointDef.upperAngle =  (45* DEGTORAD);
			(b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			
			bd.position.Set(10.0f , -7.0f);
			piston.SetAsBox(5,0.5);
			m_piston2 = m_world->CreateBody(&bd);
			m_piston2->CreateFixture(&fdpiston);
			 
			b2DistanceJointDef jointDef;
			jointDef.bodyA = m_frontarm1;
			jointDef.bodyB = m_piston2;
			jointDef.localAnchorA.Set(-1,0);
			jointDef.localAnchorB.Set(4.5,0);
			jointDef.length = 0;
			jointDef.collideConnected = true;
			jointDef.dampingRatio = 0.5f;
			(b2DistanceJoint*)m_world->CreateJoint( &jointDef );
			
			b2PrismaticJointDef prismaticJointDef;
			prismaticJointDef.bodyA = m_piston1;
			prismaticJointDef.bodyB = m_piston2;
			prismaticJointDef.localAxisA.Set(1,0);
			prismaticJointDef.localAnchorA.Set(2.5,0);
			prismaticJointDef.localAnchorB.Set(-2.5,0);
			prismaticJointDef.enableMotor = true;
			prismaticJointDef.maxMotorForce = 7.0f;
			prismaticJointDef.enableLimit = true;
			prismaticJointDef.lowerTranslation = -3.5;
			prismaticJointDef.upperTranslation = 3;
			//prismaticJointDef.motorSpeed = -100;
			prismaticJointDef.collideConnected = false;
			m_pistonjoint = (b2PrismaticJoint *) m_world->CreateJoint( &prismaticJointDef);
			
			}
				
			
			////testbox
			//{
			//b2PolygonShape testbox;
			//testbox.SetAsBox(1.4,1.4);
			//b2FixtureDef fdtestbox;
			//fdtestbox.filter.groupIndex = 1;
			//fdtestbox.shape = &testbox;
			//fdtestbox.density = 0.1f;
			//fdtestbox.friction=0.5;
			//b2BodyDef bd;
			//bd.position.Set(40.0f, -19.0f);
			//bd.type = b2_dynamicBody;
			//test_box1 = m_world->CreateBody(&bd);
			//test_box1->CreateFixture(&fdtestbox);
			//}
			
			// test boxes 
			
			
			//testball
			{
			b2CircleShape circle;
			circle.m_radius = 2.5f;
			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 0.2f;
			fd.friction = 0.9f;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-35.0f, -16.5f);
			test_ball = m_world->CreateBody(&bd);
			test_ball->CreateFixture(&fd);
			}
			
			//fixed obstacle
			//{
			//b2PolygonShape testbox;
			//testbox.SetAsBox(0.1,1);
			//b2FixtureDef fdtestbox;
			//fdtestbox.shape = &testbox;
			//b2BodyDef bd;
			//bd.position.Set(-33.0f, -19);
			//obs = m_world->CreateBody(&bd);
			//obs->CreateFixture(&fdtestbox);
			//}
			
		}
	}
	void dominos_t::mouse_down(const b2Vec2& p) {
				b2PolygonShape testbox;
				testbox.SetAsBox(1.4,1.4);
				b2FixtureDef fdtestbox;
				fdtestbox.filter.groupIndex = 1;
				fdtestbox.shape = &testbox;
				fdtestbox.density = 0.1f;
				fdtestbox.friction=0.5;
				b2Body* box=NULL;
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(p.x, p.y);
				box = m_world->CreateBody(&bd);
				box->CreateFixture(&fdtestbox);
		
		
	}
    
	float x_car,y_car;
    void dominos_t::keyboard(unsigned char key)
    {
        switch (key)
		{
		case 'a':
			x_car=m_car->GetPosition().x;
			y_car=m_car->GetPosition().y;
			m_spring1->SetMotorSpeed(m_speed);
			callbacks_t::keyboard_special1_cb(GLUT_KEY_LEFT,x_car,y_car);
			break;

		case 'd':
			x_car=m_car->GetPosition().x;
			y_car=m_car->GetPosition().y;
			m_spring1->SetMotorSpeed(-m_speed);
			callbacks_t::keyboard_special1_cb(GLUT_KEY_LEFT,x_car,y_car);
			break;
		
		case '1':
			m_mjoint1->SetMotorSpeed(-1.0f);
			break;	
			
		case '2':
			m_mjoint1->SetMotorSpeed(0.2f);
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
			m_pistonjoint->SetMaxMotorForce(5000.0f);
			m_pistonjoint->SetMotorSpeed(10.0f);
			break;
			
		case '8':	
			m_pistonjoint->SetMaxMotorForce(5000.0f);
			m_pistonjoint->SetMotorSpeed(-10.0f);
			break;
		
		case '9':	
			m_mjoint6->SetMotorSpeed(-1.0f);
			break;
		
		case '0':	
			m_mjoint6->SetMotorSpeed(1.0f);
			break;	
		}
    }
	void dominos_t::keyboard_up(unsigned char key)
    {
        switch (key)
		{
		case 'a':
		case 'd':
			x_car=m_car->GetPosition().x;
			y_car=m_car->GetPosition().y;
			m_spring1->SetMotorSpeed(0.0f);
			callbacks_t::keyboard_special1_cb(GLUT_KEY_LEFT,x_car,y_car);
			break;
		
		case '1':			
		case '2':
			m_mjoint1->SetMotorSpeed(0.0f);
			break;
			
		case '3':
		case '4':
			m_mjoint2->SetMotorSpeed(0.0f);
			break;
			
		case '5':
		case '6':	
			m_mjoint4->SetMotorSpeed(0.0f);
			break;
			
		case '7':
		case '8':
			m_pistonjoint->SetMotorSpeed(0.0f);
			break;
		
		case '9':
		case '0':	
			m_mjoint6->SetMotorSpeed(0.0f);
			break;	
		}
	}

  sim_t *sim = new sim_t("Dominos", dominos_t::create);
}
