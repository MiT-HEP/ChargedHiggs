from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout, Flatten, BatchNormalization, Input #, Merge
from keras.layers import Convolution2D, MaxPooling2D
import keras.regularizers as regularizers

def BuildModel(features,targets=1):
    model=Sequential()
    model.add(Dense(60, 
        input_dim=len(features),
        use_bias=False,
        #kernel_initializer='glorot_normal',
        #kernel_regularizer=regularizers.l2(0.01),
        ) )
    model.add(BatchNormalization())
    model.add(Activation('relu'))

    model.add(Dropout(0.10))
    model.add(Dense(10,activation='sigmoid',
        kernel_initializer='glorot_normal',
        kernel_regularizer=regularizers.l2(0.01),
        ))

    model.add(Dense(targets,activation='sigmoid'))

    model.compile(
            #loss='mean_squared_error',
            #loss='binary_crossentropy',
            loss='categorical_crossentropy',
            #loss=roc_area,
            optimizer='sgd',
            #optimizer='adam',
            metrics=['accuracy'])
    return model
