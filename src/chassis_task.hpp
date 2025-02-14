#ifndef CHASSIS_TASK_HPP
#define CHASSIS_TASK_HPP

#include "librm.hpp"

#include "bsp_dwt.h"

#include "vmc.hpp"
#include "utils.hpp"

class Chassis {
 public:
  Chassis();
  ~Chassis() = default;

  // 电机角度初始化
  void InitAngle(const f32 lf_init_angle, const f32 rf_init_angle, const f32 lb_init_angle, const f32 rb_init_angle) {
    this->lf_init_angle_ = lf_init_angle;
    this->rf_init_angle_ = rf_init_angle;
    this->lb_init_angle_ = lb_init_angle;
    this->rb_init_angle_ = rb_init_angle;
  }

  // 设置六个状态向量中的四个，剩余两个由vmc计算得到
  void SetPitch(const f32 pitch) { this->pitch_ = pitch; }
  void SetPitchDot(const f32 pitch_dot) { this->pitch_dot_ = pitch_dot; }
  void SetX(const f32 x) { this->x_ = x; }
  void SetXDot(const f32 x_dot) { this->x_dot_ = x_dot; }

  void SetYawDot(const f32 yaw_dot) { this->yaw_dot_ = yaw_dot; }

  // 更新
  void Update(const f32 left_leg_phi1, const f32 left_leg_phi4, const f32 right_leg_phi1, const f32 right_leg_phi4,
              const f32 left_leg_w_phi1, const f32 left_leg_w_phi4, const f32 right_leg_w_phi1,
              const f32 right_leg_w_phi4);

  [[nodiscard]] f32 left_l0() { return this->left_leg_vmc_.l0(); }
  [[nodiscard]] f32 right_l0() { return this->right_leg_vmc_.l0(); }

  [[nodiscard]] f32 left_phi0() { return this->left_leg_vmc_.phi0(); }
  [[nodiscard]] f32 right_phi0() { return this->right_leg_vmc_.phi0(); }

  [[nodiscard]] f32 left_leg_phi1() { return this->left_leg_phi1_; }
  [[nodiscard]] f32 left_leg_phi4() { return this->left_leg_phi4_; }
  [[nodiscard]] f32 right_leg_phi1() { return this->right_leg_phi1_; }
  [[nodiscard]] f32 right_leg_phi4() { return this->right_leg_phi4_; }

  [[nodiscard]] f32 left_leg_w_phi1() { return this->left_leg_w_phi1_; }
  [[nodiscard]] f32 left_leg_w_phi4() { return this->left_leg_w_phi4_; }
  [[nodiscard]] f32 right_leg_w_phi1() { return this->right_leg_w_phi1_; }
  [[nodiscard]] f32 right_leg_w_phi4() { return this->right_leg_w_phi4_; }

  // 设置位移目标值或速度目标值
  void SetCtrlParam(const f32 x_target, const f32 x_dot_target, const f32 wz, const f32 l0_target) {
    this->x_target_ = x_target;
    this->x_dot_target_ = x_dot_target;
    this->wz_ = wz;
    this->l0_target_ = l0_target;
  }

  // test
  f32 lf_tau() { return this->lf_tau_; }
  f32 rf_tau() { return this->rf_tau_; }
  f32 lb_tau() { return this->lb_tau_; }
  f32 rb_tau() { return this->rb_tau_; }

  f32 left_wheel_tau() { return this->left_wheel_tau_; }
  f32 right_wheel_tau() { return this->right_wheel_tau_; }

  f32 left_force() { return this->left_force_; }
  f32 right_force() { return this->right_force_; }

  f32 x() { return this->x_; }
  f32 x_dot() { return this->x_dot_; }

  f32 pitch() { return this->pitch_; }
  f32 pitch_dot() { return this->pitch_dot_; }

  f32 yaw_dot() { return this->yaw_dot_; }

  f32 left_theta() { return this->left_theta_; }
  f32 left_theta_dot() { return this->left_theta_dot_; }
  f32 right_theta() { return this->right_theta_; }
  f32 right_theta_dot() { return this->right_theta_dot_; }

  f32 left_tp() { return this->left_tau_(1); }
  f32 right_tp() { return this->right_tau_(1); }

  f32 left_wheel_t() { return this->left_wheel_tau_; }
  f32 right_wheel_t() { return this->right_wheel_tau_; }

  // 电机编码器初始化标志位
  bool A1_init_flag_;

  // 电机初始角度
  f32 lf_init_angle_, rf_init_angle_, lb_init_angle_, rb_init_angle_;

 private:
  // 设置电机角度偏置
  void LegPosTrans();

  // 将电机角度反馈传入vmc
  void Param2Vmc();

  // 状态向量转换，与模型中的状态向量对应
  void StateTrans();

  // 通过vmc的雅可比矩阵将虚拟力矩转换为实际电机输出力矩
  void Visual2Real();

  // 计算模型虚拟力矩
  void CalcTau();

  // 计算拟合多项式
  f32 CalcPoly(f32 length, int j, int i);

 private:
  // 腿长pid
  // rm::modules::algorithm::PID left_l0_pid_;
  // rm::modules::algorithm::PID right_l0_pid_;
  UserPid left_l0_pid_;
  UserPid right_l0_pid_;

  UserPid left_turn_pid_;
  UserPid right_turn_pid_;

  // 一阶低通滤波
  FirstOrderFilter lf_pos_filter_, lb_pos_filter_, rf_pos_filter_, rb_pos_filter_;
  FirstOrderFilter lf_spd_filter_, lb_spd_filter_, rf_spd_filter_, rb_spd_filter_;
  FirstOrderFilter pitch_filter_, pitch_dot_filter_;
  FirstOrderFilter yaw_dot_filter_;

  // 五连杆参数
  f32 left_leg_phi1_, left_leg_phi4_;
  f32 right_leg_phi1_, right_leg_phi4_;
  f32 left_leg_w_phi1_, left_leg_w_phi4_;
  f32 right_leg_w_phi1_, right_leg_w_phi4_;

  // 双腿vmc（包含运动学正解）
  Vmc left_leg_vmc_;
  Vmc right_leg_vmc_;

  // 六个状态向量
  f32 left_theta_, left_theta_dot_;    // 左腿摆杆角度
  f32 right_theta_, right_theta_dot_;  // 右腿摆杆角度
  f32 x_, x_dot_;                      // 位移和速度
  f32 pitch_, pitch_dot_;              // pitch轴角度和角速度

  // 两个控制向量
  Eigen::Vector2f left_tau_, right_tau_;

  // 支持力
  f32 left_force_, right_force_;

  // 实际电机输出力矩
  f32 lf_tau_, rf_tau_, lb_tau_, rb_tau_;
  f32 left_wheel_tau_, right_wheel_tau_;

  // 位移目标值或速度目标值
  f32 x_target_, x_dot_target_;

  // 转向角速度
  f32 wz_;

  // yaw轴角速度
  f32 yaw_dot_;

  // 腿长目标值
  f32 l0_target_;

  // lqr控制参数
  f32 ctrl_kp[6][2][4] = {
-93.204472,121.882184,-98.467903,-1.872395,
450.369189,-409.402373,119.698815,9.128763,
16.603582,-17.855148,-6.886850,-0.299722,
32.562755,-37.265795,17.622450,0.970508,
-86.151202,95.312531,-39.891274,-2.216988,
15.926325,40.229524,-53.987725,23.203188,
-36.331649,47.134062,-26.507296,-2.426470,
1.834064,30.359385,-35.779309,17.062761,
18.564091,53.792331,-70.385834,30.144587,
674.234833,-744.968855,311.449624,16.858126,
-23.754635,28.862392,-15.304325,4.742555,
125.088880,-129.344015,50.616403,-2.443615};

  f32 theta_kp = 13.5f;
  f32 theta_kd = 0.8f;

  f32 roll_kp = 1.2f;
  f32 roll_kd = 0.03f;
};

#endif