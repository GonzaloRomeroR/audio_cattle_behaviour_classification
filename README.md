### Audio cattle behavior classification

This project has the goal of classifying audio files obtained by sensors attached to cow heads. The audios files to classify contains useful information to determine if the cow is biting, biting while chewing or chewing. Based on that information, a model is trained to succesfully classify the three behaviours.

The idea is to generate a simple model than can be run by an embedded system. In this case an ESP32 was used to replicate the algorithms develop in python. To do so we implemeted a simple decision tree, since it provided a simple way of calssify using the embedded system. In future work, more models will by tested to have a better performace.

