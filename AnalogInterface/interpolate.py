#!/usr/bin/env python3
#
# Compute bins for y = -0.8285714 + 1.846286*x - 0.008342857*x^2
#
# There are 11 bins (off and 10 cooling power settings) mapped to the input
# duty cycle curve, with a 1% hysteresis to minimise setting hunting.
#
# Each bin has a "step down if lower" and "step up if higher" threshold, obviously
# there is no down for off, or up for max.
#
def duty(point):
    if (point < 0):
        point = 0
    if (point > 100):
        point = 100
    return int(-0.8285714 + 1.846286 * point - 0.008342857 * (point ** 2))

bincount = 10

step = 100.0 / bincount
for bin_number in range(0, bincount):
    low_point = 0 if bin_number == 0 else int((bin_number * step) - 1)
    high_point = 100 if bin_number == 10 else int(((bin_number + 1) * step) + 1)
    low_duty = duty(low_point)
    high_duty = duty(high_point)
    print(f'{bin_number}: {low_point}..{high_point}  {low_duty}..{high_duty}')
