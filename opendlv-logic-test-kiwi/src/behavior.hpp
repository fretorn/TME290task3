/*
 * Copyright (C) 2018 Ola Benderius
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BEHAVIOR
#define BEHAVIOR

#include <mutex>

#include "opendlv-standard-message-set.hpp"

class Behavior {
 private:
  Behavior(Behavior const &) = delete;
  Behavior(Behavior &&) = delete;
  Behavior &operator=(Behavior const &) = delete;
  Behavior &operator=(Behavior &&) = delete;

 public:
  Behavior() noexcept;
  ~Behavior() = default;

 public:
  opendlv::proxy::GroundSteeringRequest getGroundSteeringAngle() noexcept;
  opendlv::proxy::PedalPositionRequest getPedalPositionRequest() noexcept;
  opendlv::proxy::DistanceReading getFrontUltrasonic() noexcept;
  opendlv::proxy::DistanceReading getRearUltrasonic() noexcept;
  opendlv::logic::sensation::Point getCameraEstimation() noexcept;
  double getLeftIr() noexcept;
  void setFrontUltrasonic(opendlv::proxy::DistanceReading const &) noexcept;
  void setRearUltrasonic(opendlv::proxy::DistanceReading const &) noexcept;
  void setLeftIr(opendlv::proxy::VoltageReading const &) noexcept;
  void setRightIr(opendlv::proxy::VoltageReading const &) noexcept;
  // void setAzimuthAngle(opendlv::logic::sensation::Point const &) noexcept;
  // void setDistance(opendlv::logic::sensation::Point const &) noexcept;
  void setAzimuthAngle(float const &) noexcept;
  void setDistance(float const &) noexcept;
  void step(float speed, float front, float rear, float goalDistanceToWall, 
  float sideWall, float reverseTimeThreshold, float groundSteering, 
  float wallSteering, float rearMin, float reverseSpeed, float FREQ,
   float Kp_side, float sideDistanceForStraightReverse,
    float forwardTimeAfterReverseLimit, float addAngleAfterReverse) noexcept;

 private:
  double convertIrVoltageToDistance(float) const noexcept;

 private:
  opendlv::proxy::DistanceReading m_frontUltrasonicReading;
  opendlv::proxy::DistanceReading m_rearUltrasonicReading;
  opendlv::proxy::VoltageReading m_leftIrReading;
  opendlv::proxy::VoltageReading m_rightIrReading;
  opendlv::proxy::GroundSteeringRequest m_groundSteeringAngleRequest;
  opendlv::proxy::PedalPositionRequest m_pedalPositionRequest;
  float m_azimuthAngle;
  float m_cameraDistance;
  std::mutex m_frontUltrasonicReadingMutex;
  std::mutex m_rearUltrasonicReadingMutex;
  std::mutex m_leftIrReadingMutex;
  std::mutex m_rightIrReadingMutex;
  std::mutex m_groundSteeringAngleRequestMutex;
  std::mutex m_pedalPositionRequestMutex;
  std::mutex m_azimuthAngleMutex;
  std::mutex m_cameraDistanceMutex;
};

// extern float speed;

#endif
