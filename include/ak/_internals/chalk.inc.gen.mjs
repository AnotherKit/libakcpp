import fs from 'fs'
import { qw } from '@alan-liang/qw'

const colors = [ 'reset', 'bold', 'dim', 'italic', 'underline', 'inverse', 'hidden', 'strikethrough', 'black', 'red', 'green', 'yellow', 'blue', 'magenta', 'cyan', 'white', 'gray', 'bgBlack', 'bgRed', 'bgGreen', 'bgYellow', 'bgBlue', 'bgMagenta', 'bgCyan', 'bgWhite', 'blackBright', 'redBright', 'greenBright', 'yellowBright', 'blueBright', 'magentaBright', 'cyanBright', 'whiteBright', 'bgBlackBright', 'bgRedBright', 'bgGreenBright', 'bgYellowBright', 'bgBlueBright', 'bgMagentaBright', 'bgCyanBright', 'bgWhiteBright' ]

qw(colors)
  .map(x => `auto ${x} (const std::string &str) -> std::string;`)
  .join('\n')
  ._(x => fs.writeFileSync(new URL('chalk.inc', import.meta.url), x))
