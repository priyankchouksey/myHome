var exec = require('child_process').execSync;

console.log("Starting ng Build");
process.chdir('./ngSrc');
exec("ng build");
console.log("Completed ng build");