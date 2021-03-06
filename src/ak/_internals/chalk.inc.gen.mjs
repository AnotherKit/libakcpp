// https://github.com/jorgebucaran/colorette/blob/840f50e7ed49bc79a7a373d588a2eda66dd94d15/index.js
// SPDX-License-Identifier: MIT

import fs from 'fs'
import { qw } from '@alan-liang/qw'

const init = (...args) => args

const colors = {
  reset: init(0, 0),
  bold: init(1, 22, "\x1b[22m\x1b[1m"),
  dim: init(2, 22, "\x1b[22m\x1b[2m"),
  italic: init(3, 23),
  underline: init(4, 24),
  inverse: init(7, 27),
  hidden: init(8, 28),
  strikethrough: init(9, 29),
  black: init(30, 39),
  red: init(31, 39),
  green: init(32, 39),
  yellow: init(33, 39),
  blue: init(34, 39),
  magenta: init(35, 39),
  cyan: init(36, 39),
  white: init(37, 39),
  gray: init(90, 39),
  bgBlack: init(40, 49),
  bgRed: init(41, 49),
  bgGreen: init(42, 49),
  bgYellow: init(43, 49),
  bgBlue: init(44, 49),
  bgMagenta: init(45, 49),
  bgCyan: init(46, 49),
  bgWhite: init(47, 49),
  blackBright: init(90, 39),
  redBright: init(91, 39),
  greenBright: init(92, 39),
  yellowBright: init(93, 39),
  blueBright: init(94, 39),
  magentaBright: init(95, 39),
  cyanBright: init(96, 39),
  whiteBright: init(97, 39),
  bgBlackBright: init(100, 49),
  bgRedBright: init(101, 49),
  bgGreenBright: init(102, 49),
  bgYellowBright: init(103, 49),
  bgBlueBright: init(104, 49),
  bgMagentaBright: init(105, 49),
  bgCyanBright: init(106, 49),
  bgWhiteBright: init(107, 49),
}

const makeFunction = (name, open, close, replace) => `
auto ${name} (const std::string &str) -> std::string {
  return filterEmpty(${JSON.stringify(`\x1b[${open}m`)}, ${JSON.stringify(`\x1b[${close}m`)}, str${replace ? ', ' + JSON.stringify(replace) : ''});
}
`

qw(colors)
  ._(x => Object.entries(x))
  .map(([ name, args ]) => makeFunction(name, ...args))
  .join('')
  ._(x => fs.writeFileSync(new URL('chalk.inc', import.meta.url), x))
