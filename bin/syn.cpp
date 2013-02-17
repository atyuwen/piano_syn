float v = 0.3 * sin(0.5 * w * t) * exp(-20 * t);
v += 0.8 * sin(w * t) * exp(-30 * t);
v += 2 * sin(2 * w * t) * exp(-4 * t);
v += 1 * sin(3 * w * t) * exp(-4 * t);
return v;