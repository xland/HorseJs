let esbuild = require("esbuild");
const { dtsPlugin } = require("esbuild-plugin-d.ts");

let startFunc = async () => {
  let ctx = await esbuild.context({
    entryPoints: [`Horse.ts`],
    bundle: true,
    outfile: "../Res/horse.js",
    platform: "browser",
    sourcemap: true,
    target: "esnext",
    plugins: [dtsPlugin()],
  });
  await ctx.watch();
};
startFunc();
