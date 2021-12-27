qw
===
**Warning**: experimental, uses `Proxy` which is not polyfillable.

Data pipeline utility.

## Install
```shell
npm install @alan-liang/qw
```

## Usage
```javascript
import { qw } from '@alan-liang/qw'

qw(Array(16))
  .fill(0)
  .map((_, i) => i)
  .reduce((a, b) => a + b)
  ._(x => x / 2)
  ._.log() // logs 60
  ._.value // => 60
```
See `example.js` for detailed usage.

