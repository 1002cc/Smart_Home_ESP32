const ffmpeg = require('fluent-ffmpeg');

function convertImagesToVideo(images, outputPath, fps = 30) {
  return new Promise((resolve, reject) => {
    const cmd = ffmpeg(images)
      .output(outputPath)
      .fps(fps)
      .on('end', () => {
        resolve('Video created successfully');
      })
      .on('error', (err) => {
        reject(err);
      });

    cmd.run();
  });
}

const images = [
  'temp/temp-0.jpg',
  'temp/temp-1.jpg',
  'temp/temp-2.jpg',
  'temp/temp-3.jpg',
  'temp/temp-4.jpg',
  'temp/temp-5.jpg'
  // 添加更多图片路径
];

const outputPath = 'output.mp4';

convertImagesToVideo(images, outputPath)
  .then((message) => {
    console.log(message);
  })
  .catch((err) => {
    console.error(err);
  });
