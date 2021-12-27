
std::string reset (const std::string &str) {
  return filterEmpty("\u001b[0m", "\u001b[0m", str);
}

std::string bold (const std::string &str) {
  return filterEmpty("\u001b[1m", "\u001b[22m", str, "\u001b[22m\u001b[1m");
}

std::string dim (const std::string &str) {
  return filterEmpty("\u001b[2m", "\u001b[22m", str, "\u001b[22m\u001b[2m");
}

std::string italic (const std::string &str) {
  return filterEmpty("\u001b[3m", "\u001b[23m", str);
}

std::string underline (const std::string &str) {
  return filterEmpty("\u001b[4m", "\u001b[24m", str);
}

std::string inverse (const std::string &str) {
  return filterEmpty("\u001b[7m", "\u001b[27m", str);
}

std::string hidden (const std::string &str) {
  return filterEmpty("\u001b[8m", "\u001b[28m", str);
}

std::string strikethrough (const std::string &str) {
  return filterEmpty("\u001b[9m", "\u001b[29m", str);
}

std::string black (const std::string &str) {
  return filterEmpty("\u001b[30m", "\u001b[39m", str);
}

std::string red (const std::string &str) {
  return filterEmpty("\u001b[31m", "\u001b[39m", str);
}

std::string green (const std::string &str) {
  return filterEmpty("\u001b[32m", "\u001b[39m", str);
}

std::string yellow (const std::string &str) {
  return filterEmpty("\u001b[33m", "\u001b[39m", str);
}

std::string blue (const std::string &str) {
  return filterEmpty("\u001b[34m", "\u001b[39m", str);
}

std::string magenta (const std::string &str) {
  return filterEmpty("\u001b[35m", "\u001b[39m", str);
}

std::string cyan (const std::string &str) {
  return filterEmpty("\u001b[36m", "\u001b[39m", str);
}

std::string white (const std::string &str) {
  return filterEmpty("\u001b[37m", "\u001b[39m", str);
}

std::string gray (const std::string &str) {
  return filterEmpty("\u001b[90m", "\u001b[39m", str);
}

std::string bgBlack (const std::string &str) {
  return filterEmpty("\u001b[40m", "\u001b[49m", str);
}

std::string bgRed (const std::string &str) {
  return filterEmpty("\u001b[41m", "\u001b[49m", str);
}

std::string bgGreen (const std::string &str) {
  return filterEmpty("\u001b[42m", "\u001b[49m", str);
}

std::string bgYellow (const std::string &str) {
  return filterEmpty("\u001b[43m", "\u001b[49m", str);
}

std::string bgBlue (const std::string &str) {
  return filterEmpty("\u001b[44m", "\u001b[49m", str);
}

std::string bgMagenta (const std::string &str) {
  return filterEmpty("\u001b[45m", "\u001b[49m", str);
}

std::string bgCyan (const std::string &str) {
  return filterEmpty("\u001b[46m", "\u001b[49m", str);
}

std::string bgWhite (const std::string &str) {
  return filterEmpty("\u001b[47m", "\u001b[49m", str);
}

std::string blackBright (const std::string &str) {
  return filterEmpty("\u001b[90m", "\u001b[39m", str);
}

std::string redBright (const std::string &str) {
  return filterEmpty("\u001b[91m", "\u001b[39m", str);
}

std::string greenBright (const std::string &str) {
  return filterEmpty("\u001b[92m", "\u001b[39m", str);
}

std::string yellowBright (const std::string &str) {
  return filterEmpty("\u001b[93m", "\u001b[39m", str);
}

std::string blueBright (const std::string &str) {
  return filterEmpty("\u001b[94m", "\u001b[39m", str);
}

std::string magentaBright (const std::string &str) {
  return filterEmpty("\u001b[95m", "\u001b[39m", str);
}

std::string cyanBright (const std::string &str) {
  return filterEmpty("\u001b[96m", "\u001b[39m", str);
}

std::string whiteBright (const std::string &str) {
  return filterEmpty("\u001b[97m", "\u001b[39m", str);
}

std::string bgBlackBright (const std::string &str) {
  return filterEmpty("\u001b[100m", "\u001b[49m", str);
}

std::string bgRedBright (const std::string &str) {
  return filterEmpty("\u001b[101m", "\u001b[49m", str);
}

std::string bgGreenBright (const std::string &str) {
  return filterEmpty("\u001b[102m", "\u001b[49m", str);
}

std::string bgYellowBright (const std::string &str) {
  return filterEmpty("\u001b[103m", "\u001b[49m", str);
}

std::string bgBlueBright (const std::string &str) {
  return filterEmpty("\u001b[104m", "\u001b[49m", str);
}

std::string bgMagentaBright (const std::string &str) {
  return filterEmpty("\u001b[105m", "\u001b[49m", str);
}

std::string bgCyanBright (const std::string &str) {
  return filterEmpty("\u001b[106m", "\u001b[49m", str);
}

std::string bgWhiteBright (const std::string &str) {
  return filterEmpty("\u001b[107m", "\u001b[49m", str);
}
