<p align="center">
  <img src=img/Flexe_logo.png>
</p>

# Flexe
Flexe is a new framework for simulation of Federated Learning (FL) in Connected and Autonomous Vehicle (CAVs). Its adaptable design allows for the implementation of a variety of FL schemes, including horizontal, vertical, and Federated Transfer Learning. Flexe and PyFlexe are free to download and use, built for customization, and allows for the realistic simulation of wireless networking and vehicle dynamics. The project was partitioned into two, one of which is responsible for vehicle-to-vehicle communication ([Flexe](https://github.com/WellingtonLobato/flexe)) and the other for the construction and training of models ([PyFlexe](https://github.com/WellingtonLobato/PyFlexe)).

## Getting Started

We developed FLEXE to make it possible to implement and develop vehicular FL applications within the context of CAVs. It further simplifies the process of modeling specific Machine Learning (ML) and FL applications into environments suitable for CAVs. Specifically, we developed Flexe on top of the Veins network simulator to simulate the dynamics of communication between vehicles. 

### Prerequisites
PyFlexe requires the following software to be installed 

- OMNeT++
- conan
- grpc
- TensorFlow
- PyTorch

### OMNeT++ (6 >=) installation
Please do follow the instructions from the official [OMNeT documentation](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf)

### Conan installation (1.58.0 >=)
Please do follow the instructions from the official [conan documentation](https://docs.conan.io/en/latest/installation.html)

### GRPC installation (== 1.38.1)
Please do follow the instructions from the official [GRPC documentation](https://grpc.io/docs/languages/python/quickstart/)

### Installing

In order to install the necessary packages to run Flexe, just run the following command in the root directory.

```
conan install . --build missing -s compiler.version=10 -s compiler.libcxx=libstdc++11
```

Import the project into your OMNeT++ IDE workspace by clicking File > Import > General: Existing Projects into Workspace and selecting the directory you unpacked the Flexe module  to.

Flexe is an extension to the well known and widely used Veins simulation framework. See the Veins website <http://veins.car2x.org/> for a tutorial, documentation, and publications.


## Project structure - main components 


    ├── simulations
    │   ├── grid
    │   ├── small_lust
    │   └── veins
    └── src
        ├── flexe
        │   ├── application
        │   ├── cache
        │   ├── messages
        │   └── proto
        ├── Makefile
        └── makefrag

## Instant Flexe Virtual Machine
Instant Flexe is a virtual machine you can use to quickly try out Flexe or to run Flexe on systems you do not have administrative access to.
[Instant Flexe](https://drive.google.com/file/d/1AKOdnB7UYMtabeHEz1AlBlQEUpy-Vvdz/view?usp=sharing)

## !!WARNING!!
Make sure to check the option 'Enable Nested VT-x/AMD-V' in VirtualBox to properly run the VM. 

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

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
