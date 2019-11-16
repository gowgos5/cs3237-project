import numpy as np
import queue
import threading
import paho.mqtt.client as mqtt
from keras.models import load_model

pending_data_q = queue.Queue()
window_size = 25
has_prev_value = False
prev_value = 0
labels = ['IDLE', 'JUMP', 'RUN']

def normalise(dataset):
    mean = np.mean(dataset, axis=0)
    sd = np.std(dataset, axis=0)
    return (dataset - mean) / sd

def ml_model_thread_function():
    model = load_model('HAR_model.hdf5')
    window = np.zeros((window_size, 3))
    i = 0
    
    while (True):
        data = pending_data_q.get()
        
        window[i] = data
        i += 1
        
        if i > window_size:
            window = window[:window_size]
            i = window_size

        if i == window_size:
            # reshape and predict
            # window = normalise(window)
            window = window.reshape(1, window_size, 3)
            result = model.predict(window)

            # print result
            print(labels[np.argmax(result)])
            print(result)
            client.publish("iot/activity", labels[np.argmax(result)])

            # reset window
            window = np.zeros((window_size, 3))
            i = 0

def live_analyse():
    ml_model_thread = threading.Thread(target=ml_model_thread_function, daemon=True)
    ml_model_thread.start()

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("iot/acc")

def on_message(client, userdata, msg):
    value = np.fromstring(msg.payload.decode("utf-8"), sep=',') # x,y,z
    compute_and_queue_diff(value)

def compute_and_queue_diff(curr_value):
    global prev_value
    global has_prev_value

    if (has_prev_value):
        diff = curr_value - prev_value
        pending_data_q.put(diff)
    else:
        has_prev_value = True

    prev_value = curr_value
    return

if __name__ == '__main__':
    live_analyse()
    
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    
    print("Connecting")
    client.connect("localhost" , 1883, 60)
    client.loop_forever()
