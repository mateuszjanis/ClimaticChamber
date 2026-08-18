import datetime
from picamera2 import Picamera2, Preview
from libcamera import controls
import time

camera = Picamera2()
camera_config = camera.create_still_configuration()
#camera_config = camera.create_preview_configuration()
camera.configure(camera_config)
#camera.start_preview(Preview.QTGL)

camera.start()
camera.set_controls({"AfMode": controls.AfModeEnum.Continuous})
time.sleep(2)

delay = 10
filename = 'pyimage_'
filepath = './Documents/images/'

try:
	while True:
		date = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
		camera.capture_file(filepath + filename + date + '.jpg')
		time.sleep(delay)
except KeyboardInterrupt:
	print("Przerwano")

finally:
	camera.stop()
