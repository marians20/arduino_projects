#ifndef __UTILS_H__
#define __UTILS_H__

auto padLeft(String value, int totalLength, char symbol) -> String {
  auto count = totalLength - value.length();
  for(int i = 0; i < count; i++) {
    value = String(symbol) + value;
  }

  return value;
}

auto secondsToTimeString(int seconds) -> String {
  int hours = seconds / 3600;
  int mins = (seconds % 3600) / 60;
  int secs = (seconds % 3600 % 60);
  String minsSecs = padLeft(String(mins), 2, '0') + String(":") + padLeft(String(secs), 2, '0');
  return hours != 0 ? padLeft(String(hours), 2, '0') + String(":") + minsSecs : minsSecs;
}

#endif