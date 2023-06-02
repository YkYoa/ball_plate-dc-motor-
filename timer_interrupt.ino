void timer_interrupt() {
  x_dot = (x - x_previous) * 100;
  y1_dot = (y1 - y1_previous) * 100;
  y2_dot = (y2 - y2_previous) * 100;

  t1_dot = (t1 - t1_previous) * 100;
  t2_dot = (t2 - t2_previous) * 100;
  t3_dot = (t3 - t3_previous) * 100;

  u1 = -(k11 * error_x + k12 * x_dot + k13 * t1 + k14 * t1_dot);
  if (u1 > 12) { u1 = 12; }
  if (u1 < -12) { u1 = -12; }
  Output1 = (u1 / 12) * 255;

  u2 = -(k21 * error_y1 + k22 * y1_dot + k23 * t2 + k24 * t2_dot);
  if (u2 > 12) { u2 = 12; }
  if (u2 < -12) { u2 = -12; }
  Output2 = (u2 / 12) * 255;

  u3 = -(k31 * error_y2 + k32 * y2_dot + k33 * t3 + k34 * t3_dot);
  if (u3 > 12) { u3 = 12; }
  if (u3 < -12) { u3 = -12; }
  Output3 = (u3 / 12) * 255;

  if (Output1 >= 0) {
    if (t1 > 0.2) {
      analogWrite(PWM_1, 0);
    } else {
      digitalWrite(DC1_LEN, HIGH);
      digitalWrite(DC1_XUONG, LOW);
      analogWrite(PWM_1, abs(Output1));
    }
  } else {
    if (t1 < -0.2) {
      analogWrite(PWM_1, 0);
    } else {
      digitalWrite(DC1_LEN, LOW);
      digitalWrite(DC1_XUONG, HIGH);
      analogWrite(PWM_1, abs(Output1));
    }
  }

  if (Output2 >= 0) {
    if (t2 > 0.2) {
      analogWrite(PWM_2, 0);
    } else {
      digitalWrite(DC2_LEN, HIGH);
      digitalWrite(DC2_XUONG, LOW);
      analogWrite(PWM_2, abs(Output2));
    }
  } else {
    if (t2 < -0.2) {
      analogWrite(PWM_2, 0);
    } else {
      digitalWrite(DC2_LEN, LOW);
      digitalWrite(DC2_XUONG, HIGH);
      analogWrite(PWM_2, abs(Output2));
    }
  }

  if (Output3 >= 0) {
    if (t3 > 0.2) {
      analogWrite(PWM_3, 0);
    } else {
      digitalWrite(DC3_LEN, HIGH);
      digitalWrite(DC3_XUONG, LOW);
      analogWrite(PWM_3, abs(Output3));
    }
  } else {
    if (t3 < -0.2) {
      analogWrite(PWM_3, 0);
    } else {
      digitalWrite(DC3_LEN, LOW);
      digitalWrite(DC3_XUONG, HIGH);
      analogWrite(PWM_3, abs(Output3));
    }
  }

  x_previous = x;
  y1_previous = y1;
  y2_previous = y2;

  t1_previous = t1;
  t2_previous = t2;
  t3_previous = t3;
  setPointDegrees_1 += 1;
}