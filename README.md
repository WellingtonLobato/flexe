<p align="center">
  <img src=img/Flexe_logo.png>
</p>

# Flexe
Flexe is a new framework for simulation of Federated Learning (FL) in Connected and Autonomous Vehicle (CAVs). Its adaptable design allows for the implementation of a variety of FL schemes, including horizontal, vertical, and Federated Transfer Learning. PyFlexe is free to download and use, built for customization, and allows for the realistic simulation of wireless networking and vehicle dynamics. The project was partitioned into two, one of which is responsible for vehicle-to-vehicle communication (Flexe) and the other for the construction and training of models (PyFlexe).

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites
PyFlexe requires the following software to be installed 

- OMNeT++
- conan
- grpc
- TensorFlow
- PyTorch

### OMNeT++ (6 >=) installation
Please do follow the instructions from the official [OMNeT documentation](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf)

### Conan installation
Please do follow the instructions from the official [conan documentation](https://docs.conan.io/en/latest/installation.html)

### GRPC installation
Please do follow the instructions from the official [GRPC documentation](https://grpc.io/docs/languages/python/quickstart/)

### TensorFlow installation (2.11.0 >=)
Please do follow the instructions from the official [TensorFlow documentation](https://www.tensorflow.org/install)

### PyTorch installation (1.13.1 >=)
Please do follow the instructions from the official [PyTorch documentation](https://pytorch.org/tutorials/beginner/basics/intro.html)

### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Project structure - main components 

Each strategy is evaluated through `simulation.py` which creates the set of clients and the server of the selected strategy under the given setting.
Additionally, FedLTA can generate various visualizations to help researchers to analyze the results (see folder `analysis`).

    ├── analysis 			# Tables and plots creation - Folder
    ├── client 			# Client-side code - Folder
    │   ├── client_tf		# Client-side code for TensorFlow - Folder
    │   ├── client_torch		# Client-side code for Pytorch - Folder
    ├── data			# Datasets - Folder
    ├── dataset_utils_torch.py	# Dataset utilities for Pytorch
    ├── exc_all.sh			# Executes all the experiments
    ├── exc_joint_plot.sh		# Generates joint plots 
    ├── exec_simulation.py		# Executes the selected experiment
    ├── execution_log		# Contains the terminal logs of each executed experiment - Folder
    ├── log_exc_all.txt
    ├── logs			# Contains the results of each executed experiment - Folder
    ├── model_definition_tf.py	# Deep learning models in TensorFlow
    ├── model_definition_torch.py	# Deep learning models in Pytorch
    ├── push_logs.sh		# Pushes the generated logs to GitHub 
    ├── server			# Server-side code
    │   ├── common_base_server	# Common base server code (i.e., it is independent of Tensorflow and Pytorch) - Folder
    │   ├── server_tf		# TensorFlow-specific implementations of servers - Folder
    │   └── server_torch		# Pytorch-specific implementations of servers - Folder
    ├── simulation.py		# Main file. Executes the selected strategy in the Federated |Learning under the specified setting

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. 

## Authors

* **Wellington Lobato** - [WellingtonLobato](https://github.com/WellingtonLobato)
* **Joahannes B. D. da Costa** - [joahannes](https://github.com/joahannes)
* **Allan M. de Souza** - [AllanMSouza](https://github.com/AllanMSouza)
* **Denis Rosario**
* **Christoph Sommer** - [sommer](https://github.com/sommer)
* **Leandro A. Villas**

# Citation

PyFlexe and Flexe can reproduce results in the following papers:

```tex
@INPROCEEDINGS{Lobato2022,
  author={Lobato, Wellington and Costa, Joahannes B. D. Da and Souza, Allan M. de and Rosário, Denis and Sommer, Christoph and Villas, Leandro A.},
  booktitle={2022 IEEE 96th Vehicular Technology Conference (VTC2022-Fall)}, 
  title={FLEXE: Investigating Federated Learning in Connected Autonomous Vehicle Simulations}, 
  year={2022},
  pages={1-5},
  doi={10.1109/VTC2022-Fall57202.2022.10012905}
}
```

## License

This project is licensed under the GPL-2.0 license - see the [COPYING.md](COPYING.md) file for details
