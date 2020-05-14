from keras.callbacks import Callback

class WeightsSaver(Callback):
    def __init__(self):
        self.N = 0

    def on_batch_end(self, batch, logs={}):
        # batch restart from 0 at each epoch
        if batch == 0:
            print "Saving benchmark",self.N
            #name = 'weights%08d.h5' % self.N
            #self.model.save_weights(name)
            name = 'weights%08d.hd' % self.N
            self.model.save(name)
            self.N +=1
