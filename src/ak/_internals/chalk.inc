
auto reset (const std::string &str) -> std::string {
  return filterEmpty("\u001b[0m", "\u001b[0m", str);
}

auto bold (const std::string &str) -> std::string {
  return filterEmpty("\u001b[1m", "\u001b[22m", str, "\u001b[22m\u001b[1m");
}

auto dim (const std::string &str) -> std::string {
  return filterEmpty("\u001b[2m", "\u001b[22m", str, "\u001b[22m\u001b[2m");
}

auto italic (const std::string &str) -> std::string {
  return filterEmpty("\u001b[3m", "\u001b[23m", str);
}

auto underline (const std::string &str) -> std::string {
  return filterEmpty("\u001b[4m", "\u001b[24m", str);
}

auto inverse (const std::string &str) -> std::string {
  return filterEmpty("\u001b[7m", "\u001b[27m", str);
}

auto hidden (const std::string &str) -> std::string {
  return filterEmpty("\u001b[8m", "\u001b[28m", str);
}

auto strikethrough (const std::string &str) -> std::string {
  return filterEmpty("\u001b[9m", "\u001b[29m", str);
}

auto black (const std::string &str) -> std::string {
  return filterEmpty("\u001b[30m", "\u001b[39m", str);
}

auto red (const std::string &str) -> std::string {
  return filterEmpty("\u001b[31m", "\u001b[39m", str);
}

auto green (const std::string &str) -> std::string {
  return filterEmpty("\u001b[32m", "\u001b[39m", str);
}

auto yellow (const std::string &str) -> std::string {
  return filterEmpty("\u001b[33m", "\u001b[39m", str);
}

auto blue (const std::string &str) -> std::string {
  return filterEmpty("\u001b[34m", "\u001b[39m", str);
}

auto magenta (const std::string &str) -> std::string {
  return filterEmpty("\u001b[35m", "\u001b[39m", str);
}

auto cyan (const std::string &str) -> std::string {
  return filterEmpty("\u001b[36m", "\u001b[39m", str);
}

auto white (const std::string &str) -> std::string {
  return filterEmpty("\u001b[37m", "\u001b[39m", str);
}

auto gray (const std::string &str) -> std::string {
  return filterEmpty("\u001b[90m", "\u001b[39m", str);
}

auto bgBlack (const std::string &str) -> std::string {
  return filterEmpty("\u001b[40m", "\u001b[49m", str);
}

auto bgRed (const std::string &str) -> std::string {
  return filterEmpty("\u001b[41m", "\u001b[49m", str);
}

auto bgGreen (const std::string &str) -> std::string {
  return filterEmpty("\u001b[42m", "\u001b[49m", str);
}

auto bgYellow (const std::string &str) -> std::string {
  return filterEmpty("\u001b[43m", "\u001b[49m", str);
}

auto bgBlue (const std::string &str) -> std::string {
  return filterEmpty("\u001b[44m", "\u001b[49m", str);
}

auto bgMagenta (const std::string &str) -> std::string {
  return filterEmpty("\u001b[45m", "\u001b[49m", str);
}

auto bgCyan (const std::string &str) -> std::string {
  return filterEmpty("\u001b[46m", "\u001b[49m", str);
}

auto bgWhite (const std::string &str) -> std::string {
  return filterEmpty("\u001b[47m", "\u001b[49m", str);
}

auto blackBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[90m", "\u001b[39m", str);
}

auto redBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[91m", "\u001b[39m", str);
}

auto greenBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[92m", "\u001b[39m", str);
}

auto yellowBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[93m", "\u001b[39m", str);
}

auto blueBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[94m", "\u001b[39m", str);
}

auto magentaBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[95m", "\u001b[39m", str);
}

auto cyanBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[96m", "\u001b[39m", str);
}

auto whiteBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[97m", "\u001b[39m", str);
}

auto bgBlackBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[100m", "\u001b[49m", str);
}

auto bgRedBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[101m", "\u001b[49m", str);
}

auto bgGreenBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[102m", "\u001b[49m", str);
}

auto bgYellowBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[103m", "\u001b[49m", str);
}

auto bgBlueBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[104m", "\u001b[49m", str);
}

auto bgMagentaBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[105m", "\u001b[49m", str);
}

auto bgCyanBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[106m", "\u001b[49m", str);
}

auto bgWhiteBright (const std::string &str) -> std::string {
  return filterEmpty("\u001b[107m", "\u001b[49m", str);
}
