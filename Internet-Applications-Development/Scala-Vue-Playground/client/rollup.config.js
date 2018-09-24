import vue from 'rollup-plugin-vue';
import when from 'rollup-plugin-conditional';
import { terser } from 'rollup-plugin-terser';
import multiEntry from 'rollup-plugin-multi-entry';

import css from 'rollup-plugin-css-only';
import postcss from './postcss-runner';
import postcssPresetEnv from 'postcss-preset-env';

const production = process.env.NODE_ENV === "production",
      development = !production;

const appBundle = {
  input: './src/app.js',
  output: {
    file: './dist/app.js',
    format: 'iife',
    sourcemap: (development ? 'inline' : false)
  },
  plugins: [
    css({
      output: (css) => postcss(css, './dist/app.css', [
        postcssPresetEnv({
          features: {
            'custom-properties': { preserve: false },
            'nesting-rules': true
          }
        })
      ])
    }),
    vue({ css: false, }),
    when(production, [
      terser()
    ])
  ]
}; 

const vendorBundle = {
  input: [
    './node_modules/vue/dist/vue.runtime.min.js'
  ],
  output: {
    file: './dist/vendor.js',
    format: 'es'
  },
  context: 'window',
  plugins: [
    multiEntry(),
    terser()
  ]
};

export default [appBundle, vendorBundle];
