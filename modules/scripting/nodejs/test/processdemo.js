console.log('__filename', __filename);
console.log('__dirname', __dirname);

console.log('process.argv', process.argv);

console.log('process.env', process.env);

if(module === require.main) {
  console.log('This is the main module being run.');
}

