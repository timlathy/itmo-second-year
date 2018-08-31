import replace from 'rollup-plugin-replace';
import resolve from 'rollup-plugin-node-resolve';
import commonjs from 'rollup-plugin-commonjs';
import { terser } from "rollup-plugin-terser";

export default {
  input: 'src/Main.bs.js',
  output: {
    file: 'dist/app.js',
    format: 'iife',
    sourcemap: false
  },
  plugins: [
    resolve({ jsnext: true, main: true }),
    commonjs({ sourceMap: false }),
    replace({ 'process.env.NODE_ENV': JSON.stringify(process.env.NODE_ENV) }),
    terser()
  ]
};
