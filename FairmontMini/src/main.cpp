/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Nathanael Oliver                                          */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Robot Auton and Control for Fairmont Competition          */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// LeftArm              motor         1               
// RightArm             motor         10              
// LeftChassis0         motor         5               
// LeftChassis1         motor         4               
// LeftChassis2         motor         3               
// RightChassis0        motor         19              
// RightChassis1        motor         20              
// RightChassis2        motor         17              
// FrontClaw            motor         2               
// BackClaw             motor         7               
// Intake               motor         9               
// BackClaw2            motor         11              
// Gyro                 inertial      16              
// Controller2          controller                    
// ---- END VEXCODE CONFIGURED DEVICES ----

using namespace vex;

// A global instance of competition
competition Competition;


intakeDirection intakeState = stopped;
bool clawLimit = true;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/




void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  BackClaw.setStopping(hold);
  FrontClaw.setStopping(hold);
  LeftArm.setStopping(brake);
  RightArm.setStopping(brake);
  RightChassis0.setStopping(coast);
  RightChassis1.setStopping(coast);
  RightChassis2.setStopping(coast);
  LeftChassis0.setStopping(coast);
  LeftChassis1.setStopping(coast);
  LeftChassis2.setStopping(coast);
  LeftChassis0.setVelocity(100, percent);
  LeftChassis1.setVelocity(100, percent);
  LeftChassis2.setVelocity(100, percent);
  RightChassis0.setVelocity(100, percent);
  RightChassis1.setVelocity(100, percent);
  RightChassis2.setVelocity(100, percent);
  BackClaw.setVelocity(100, percent);
  BackClaw2.setVelocity(100, percent);
  FrontClaw.setPosition(0, degrees);

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

void driveForward(double dist, bool waiting) {
  RightChassis0.spinFor(dist / WHEEL_RADIUS * M_1_PI, turns, false);
  LeftChassis0.spinFor(dist / WHEEL_RADIUS * M_1_PI, turns, false);
  RightChassis1.spinFor(dist / WHEEL_RADIUS * M_1_PI, turns, false);
  LeftChassis1.spinFor(dist / WHEEL_RADIUS * M_1_PI, turns, false);
  RightChassis2.spinFor(dist / WHEEL_RADIUS * M_1_PI, turns, false);
  LeftChassis2.spinFor(dist / WHEEL_RADIUS * M_1_PI, turns, waiting);
}

void backGrab() {
  BackClaw.spinFor(1.3, turns, false);
  BackClaw2.spinFor(1.3, turns, true);
}

void frontGrab() {
  FrontClaw.setVelocity(100, percent);
  FrontClaw.spinFor(1, turns, false);
}

void liftArm(bool waiting) {
  LeftArm.spinFor(1, turns, false);
  RightArm.spinFor(1, turns, waiting);
}

void chassisTurn (double deg, turnType dir) {
  float error = deg;
  float prevError = deg;
  float totalError = 0;
  const float threshold = 2.0;
  const float kp = 0.50;
  const float kd = 0.12;
  const float ki = 0.00;
  Gyro.setRotation(0, degrees);
  while (fabs(error) > threshold ||fabs (prevError) > threshold) {
    int speed = kp*error+kd*(prevError-error) + ki*totalError;
    LeftChassis0.spin(dir == left?reverse:forward, speed, percent);
    LeftChassis1.spin(dir == left?reverse:forward, speed, percent);
    LeftChassis2.spin(dir == left?reverse:forward, speed, percent);
    RightChassis0.spin(dir == right?reverse:forward, speed, percent);
    RightChassis1.spin(dir == right?reverse:forward, speed, percent);
    RightChassis2.spin(dir == right?reverse:forward, speed, percent);
    wait(200, msec);
    prevError = error;
    error = deg - fabs(Gyro.rotation());
    if (fabs(error) < 10) {
      totalError = totalError + error;
    }
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous() {
  /************Back up auton*************
  // Drives backwards and grabs the first
  // neutral goal with the back clamp
  driveForward(-22.5);
  driveForward(-4, false);
  backGrab();
  driveForward(15);
  **************************************/
  //************Potential Auton***********
  // Grabs the neutral mobile goal with
  // the Front claw
  driveForward(20);
  driveForward(6, false);
  frontGrab();
  liftArm();
  driveForward(-13);
  chassisTurn(90, left);
  driveForward(-8);
  backGrab();
  wait(1000, msec);
  toggleIntake();
  driveForward(5);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void leftChassisSpin(int speed, brakeType type) {
  if (abs(speed) > 5) {
    LeftChassis0.spin(forward, speed, percent);
    LeftChassis1.spin(forward, speed, percent);
    LeftChassis2.spin(forward, speed, percent);
  } else {
    LeftChassis0.stop(type);
    LeftChassis1.stop(type);
    LeftChassis2.stop(type);
  }
}
void rightChassisSpin(int speed, brakeType type ) {
  if (abs(speed) > 5) {
    RightChassis0.spin(forward, speed, percent);
    RightChassis1.spin(forward, speed, percent);
    RightChassis2.spin(forward, speed, percent);
  } else {
    RightChassis0.stop(type);
    RightChassis1.stop(type);
    RightChassis2.stop(type);
  }
}
void moveArm(bool up, bool down, int speed, brakeType type) {
  if (up) {
    LeftArm.spin(forward, speed, percent);
    RightArm.spin(forward, speed, percent);
  } else if (down) {
    LeftArm.spin(reverse, speed, percent);
    RightArm.spin(reverse, speed, percent);
  } else {
    LeftArm.stop(type);
    RightArm.stop(type);
  }
}
void toggleIntake() {
  intakeState = (intakeState == intake ? stopped : intake);
  if (intakeState == intake) {
    Intake.spin(forward, 100, percent);
  } else {
    Intake.stop(coast);
  }
}
void toggleOuttake() {
  intakeState = (intakeState == outtake ? stopped : outtake);
  if (intakeState == outtake) {
    Intake.spin(reverse, 100, percent);
  } else {
    Intake.stop(coast);
  }
}
void frontClaw(bool close, bool open) {
  if (close) {
    FrontClaw.spin(forward, 80, percent);
  } else if (open && (FrontClaw.position(degrees) >=0 || !clawLimit)) {
    FrontClaw.spin(reverse, 80, percent);
  } else {
    FrontClaw.stop(hold);
  }
}
void backClaw(bool close, bool open) {
  if (close) {
    BackClaw.spin(forward, 80, percent);
    BackClaw2.spin(forward, 80, percent);
  } else if (open) {
    BackClaw.spin(reverse, 80, percent);
    BackClaw2.spin(reverse, 80, percent);
  } else {
    BackClaw.stop(hold);
    BackClaw2.spin(reverse, 80, percent);
  }
}
void p() {
  Brain.Screen.print(Gyro.heading());
  Brain.Screen.newLine();
}

void toggleLimit() {
  clawLimit = !clawLimit;
  if (clawLimit) {
    FrontClaw.setPosition(0, degrees);
  }
}

void usercontrol(void) {
  // Control for the Intake
  Controller1.ButtonA.pressed(toggleIntake);
  Controller1.ButtonB.pressed(toggleOuttake);
  Controller2.ButtonX.pressed(toggleLimit);
  // User control code here, inside the loop

  while (true) {
    // Control for chassis
    leftChassisSpin(Controller1.Axis3.value());
    rightChassisSpin(Controller1.Axis2.value());
    // Control for Lift
    // moveArm(true, false, Controller2.Axis3.value());
    moveArm(Controller2.ButtonLeft.pressing(),
            Controller2.ButtonDown.pressing());
    // Control for Front Claw
    frontClaw(Controller2.ButtonL2.pressing(), Controller2.ButtonL1.pressing());
    // Control for Back Claw
    backClaw(Controller2.ButtonR1.pressing(), Controller2.ButtonR2.pressing());
    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
