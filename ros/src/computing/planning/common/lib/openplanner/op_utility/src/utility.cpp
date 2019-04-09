/// \file utility.cpp
/// \brief General Math and Control utility functions
/// \author Hatem Darweesh
/// \date May 14, 2016

#include "op_utility/utility.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

namespace op_utility_ns
{

UtilityH::UtilityH()
{
}

UtilityH::~UtilityH()
{
}

std::string UtilityH::getHomeDirectory()
{
  struct passwd * pw = getpwuid(getuid());
  const char * homedir = pw->pw_dir;
  return std::string(homedir);
}

double UtilityH::getMomentumScaleFactor(const double & v)
{
  if (v < 0.3) {
    return 0.6;
  } else if (v < 6.4) {
    return 0.3;
  } else if (v < 20) {
    double m = 0.7 / 3.6;
    return m * (v - 6.4) + 0.3;
  } else {
    return 0.9;
  }
}

int UtilityH::getSign(double x)
{
  if (x < 0) {
    return -1;
  } else {
    return 1;
  }
}

double UtilityH::fixNegativeAngle(const double & a)
{
  double angle = 0;
  if (a < -2.0 * M_PI || a >= 2.0 * M_PI) {
    angle = fmod(a, 2.0 * M_PI);
  } else {
    angle = a;
  }

  if (angle < 0) {
    angle = 2.0 * M_PI + angle;
  }

  return angle;
}

double UtilityH::splitPositiveAngle(const double & a)
{
  double angle = a;

  if (a < -2.0 * M_PI || a >= 2.0 * M_PI) {
    angle = fmod(a, 2.0 * M_PI);
  }

  if (angle >= M_PI) {
    angle -= 2.0 * M_PI;
  } else if (angle < -M_PI) {
    angle += 2.0 * M_PI;
  }

  return angle;
}

double UtilityH::inverseAngle(const double & a)
{
  double angle = 0;
  if (a < M_PI) {
    angle = a + M_PI;
  } else {
    angle = a - M_PI;
  }

  return angle;
}

double UtilityH::angleBetweenTwoAnglesPositive(
  const double & a1,
  const double & a2)
{
  double diff = a1 - a2;
  if (diff < 0) {
    diff = a2 - a1;
  }

  if (diff > M_PI) {
    diff = 2.0 * M_PI - diff;
  }

  return diff;
}

double UtilityH::getCircularAngle(
  const double & prevContAngle,
  const double & prevAngle, const double & currAngle)
{

  double diff = currAngle - prevAngle;
  if (diff > M_PI) {
    diff = diff - 2.0 * M_PI;
  }
  if (diff < -M_PI) {
    diff = diff + 2.0 * M_PI;
  }

  double c_ang = 0;
  if (prevContAngle == 0 || fabs(diff) < M_PI_2) {
    c_ang = prevContAngle + diff;
  } else {
    c_ang = prevContAngle;
  }

  return c_ang;
}

void UtilityH::getTickCount(struct timespec & t)
{
  while (clock_gettime(0, &t) == -1) {
  }
}

double UtilityH::getTimeDiff(
  const struct timespec & old_t,
  const struct timespec & curr_t)
{
  return (curr_t.tv_sec - old_t.tv_sec) +
         ((double) (curr_t.tv_nsec - old_t.tv_nsec) / 1000000000.0);
}

double UtilityH::getTimeDiffNow(const struct timespec & old_t)
{
  struct timespec curr_t;
  getTickCount(curr_t);
  return (curr_t.tv_sec - old_t.tv_sec) +
         ((double) (curr_t.tv_nsec - old_t.tv_nsec) / 1000000000.0);
}

std::string UtilityH::getFilePrefixHourMinuteSeconds()
{
  struct timespec now_time;
  UtilityH::getTickCount(now_time);
  tm * gmtm = localtime(&now_time.tv_sec);
  std::ostringstream str;

  str << "Y" << gmtm->tm_year;
  str << "-";
  str << "M" << gmtm->tm_mon;
  str << "-";
  str << "D" << gmtm->tm_mday;
  str << "-";
  str << "H" << gmtm->tm_hour;
  str << "-";
  str << "M" << gmtm->tm_min;
  str << "-";
  str << "S" << gmtm->tm_sec;

  return str.str();
}

std::string UtilityH::getDateTimeStr()
{
  time_t now = time(0);
  char * dateStr = ctime(&now);
  std::string str(dateStr, strlen(dateStr) - 1);
  int index = str.find(" ");
  while (index > 0) {
    str.replace(index, 1, "_");
    index = str.find(" ");
  }

  index = str.find(":");
  while (index > 0) {
    str.replace(index, 1, "-");
    index = str.find(":");
  }
  return str;
}

int UtilityH::tsCompare(
  struct timespec time1, struct timespec time2,
  int micro_tolerance)
{

  if (time1.tv_sec < time2.tv_sec) {
    return -1;             /* Less than. */
  } else if (time1.tv_sec > time2.tv_sec) {
    return 1;             /* Greater than. */

  }
  long diff = time1.tv_nsec - time2.tv_nsec;
  if (diff < -micro_tolerance) {
    return -1;             /* Less than. */
  } else if (diff > micro_tolerance) {
    return 1;             /* Greater than. */
  } else {
    return 0;             /* Equal. */

  }
}

timespec UtilityH::getTimeSpec(const time_t & srcT)
{
  timespec dstT;
  dstT.tv_sec = srcT / 1000000000;
  dstT.tv_nsec = srcT - (dstT.tv_sec * 1000000000);
  return dstT;
}

time_t UtilityH::getLongTime(const struct timespec & srcT)
{
  time_t dstT;
  dstT = srcT.tv_sec * 1000000000 + srcT.tv_nsec;
  return dstT;
}

PIDController::PIDController()
: kp_{0}, kp_v_{0},
  ki_{0}, ki_v_{0},
  kd_{0}, kd_v_{0},
  pid_lim_{0}, pid_v_{0},
  upper_limit_{0}, lower_limit_{0},
  enable_limit_{false},
  accum_err_{0}, prev_err_{0},
  reset_d_{false}, reset_i_{false}
{
}

PIDController::PIDController(double kp, double ki, double kd)
  : kp_{kp}, kp_v_{0},
    ki_{ki}, ki_v_{0},
    kd_{kd}, kd_v_{0},
    pid_lim_{0}, pid_v_{0},
    upper_limit_{0}, lower_limit_{0},
    enable_limit_{false},
    accum_err_{0}, prev_err_{0},
    reset_d_{false}, reset_i_{false}
{
}

void PIDController::setLimit(double upper, double lower)
{
  upper_limit_ = upper;
  lower_limit_ = lower;
  enable_limit_ = true;
}

double PIDController::getPID(double curr_value, double target_value)
{
  double e = target_value - curr_value;
  return getPID(e);
}

double PIDController::getPID(double e)
{
  //TODO Remember to add sampling time and multiply the time elapsed by the error
  //complex PID error calculation
  //TODO //De = ( e(i) + 3*e(i-1) - 3*e(i-2) - e(i-3) ) / 6

  if (reset_i_) {
    reset_i_ = false;
    accum_err_ = 0;
  }

  if (reset_d_) {
    reset_d_ = false;
    prev_err_ = e;
  }

  if (!enable_limit_ || (pid_v_ < upper_limit_ && pid_v_ > lower_limit_)) {
    accum_err_ += e;
  }

  double edot = e - prev_err_;

  kp_v_ = kp_ * e;
  ki_v_ = ki_ * accum_err_;
  kd_v_ = kd_ * edot;

  pid_v_ = kp_v_ + ki_v_ + kd_v_;
  pid_lim_ = pid_v_;
  if (enable_limit_) {
    if (pid_v_ > upper_limit_) {
      pid_lim_ = upper_limit_;
    } else if (pid_v_ < lower_limit_) {
      pid_lim_ = lower_limit_;
    }
  }

  prev_err_ = e;

  return pid_lim_;
}

std::string PIDController::toStringHeader()
{
  std::ostringstream str_out;
  str_out << "Time" << "," << "KP" << "," << "KI" << "," << "KD" << "," <<
    "KP_v" << "," << "KI_v" << "," << "KD_v" << "," << "pid_v" << "," <<
    "," << "pid_lim" << "," << "," << "prev_err_" << "," << "accum_err_" <<
    ",";
  return str_out.str();
}

std::string PIDController::toString()
{
  std::ostringstream str_out;
  timespec t_stamp;
  UtilityH::getTickCount(t_stamp);
  str_out << UtilityH::getLongTime(t_stamp) << "," << kp_ << "," << ki_ << "," <<
    kd_ << "," << kp_v_ << "," << ki_v_ << "," << kd_v_ << "," << pid_v_ <<
    "," << "," << pid_lim_ << "," << "," << prev_err_ << "," << accum_err_ <<
    ",";

  return str_out.str();

}

void PIDController::resetD()
{
  reset_d_ = true;
}

void PIDController::resetI()
{
  reset_i_ = true;
}

void PIDController::init(double kp, double ki, double kd)
{
  kp_ = kp;
  ki_ = ki;
  kd_ = kd;
}

LowpassFilter::LowpassFilter()
{
  A = 0;
  d1 = 0;
  d2 = 0;
  w0 = 0;
  w1 = 0;
  w2 = 0;

  m = 0;
  sampleF = 0;
  cutOffF = 0;
}

LowpassFilter::~LowpassFilter()
{
//	if(A)
//		delete A;
//	if(d1)
//		delete d1;
//	if(d2)
//		delete d2;
//	if(w0)
//		delete w0;
//	if(w1)
//		delete w1;
//	if(w2)
//		delete w2;
}

LowpassFilter::LowpassFilter(
  const int & filterOrder, const double & sampleFreq,
  const double & cutOffFreq)
{
  init(filterOrder, sampleFreq, cutOffFreq);
}

void LowpassFilter::init(
  const int & filterOrder, const double & sampleFreq,
  const double & cutOffFreq)
{
  if (!(filterOrder == 2 || filterOrder == 4 || filterOrder == 6 || filterOrder == 8)) {
    std::cout << "Undefined LowpassFilter order ! " << std::endl;

    A = 0;
    d1 = 0;
    d2 = 0;
    w0 = 0;
    w1 = 0;
    w2 = 0;

    m = 0;
    sampleF = 0;
    cutOffF = 0;
  } else {
    m = filterOrder / 2;
    sampleF = sampleFreq;
    cutOffF = cutOffFreq;
    double ep = 1;
    double s = sampleFreq;
    double f = cutOffFreq;
    double a = tan(M_PI * f / s);
    double a2 = a * a;
    double u = log((1.0 + sqrt(1.0 + ep * ep)) / ep);
    double su = sinh(u / (double) filterOrder);
    double cu = cosh(u / (double) filterOrder);
    double b, c;

//		A  = new double[m];
//		d1 = new double[m];
//		d2 = new double[m];
//		w0 = new double[m];
//		w1 = new double[m];
//		w2 = new double[m];
//
//		for(int i=0; i < m ; i++)
//		{
//			A[i]  = 0;
//			d1[i] = 0;
//			d2[i] = 0;
//			w0[i] = 0;
//			w1[i] = 0;
//			w2[i] = 0;
//		}

    for (int i = 0; i < m; ++i) {
      b = sin(M_PI * (2.0 * i + 1.0) / (2.0 * filterOrder)) * su;
      c = cos(M_PI * (2.0 * i + 1.0) / (2.0 * filterOrder)) * cu;
      c = b * b + c * c;
      s = a2 * c + 2.0 * a * b + 1.0;
      A = a2 / (4.0 * s);                   // 4.0
      d1 = 2.0 * (1 - a2 * c) / s;
      d2 = -(a2 * c - 2.0 * a * b + 1.0) / s;
    }
  }
}

double LowpassFilter::getFilter(const double & value)
{
  double ep = 2.3 / 1.0;       // used to normalize
  double x = value;
  for (int i = 0; i < m; ++i) {
    w0 = d1 * w1 + d2 * w2 + x;
    x = A * (w0 + 2.0 * w1 + w2);
    w2 = w1;
    w1 = w0;
  }
  return ep * x;
}

}
