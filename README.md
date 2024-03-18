# FLEXE: Federated Learning Extension for Veins

FLEXE is a new framework for simulation of Federated Learning (FL) in Connected and Autonomous Vehicle (CAVs). Its adaptable design allows for the implementation of a variety of FL schemes, including horizontal, vertical, and Federated Transfer Learning. FLEXE is free to download and use, built for customization, and allows for the realistic simulation of wireless networking and vehicle dynamics.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

OMNeT++:
-6.0
SUMO:
-
Veins:
-5.2

```
Give examples
```

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

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Wellington Lobato** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)
* **Joahannes B. D. da Costa** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)
* **Allan M. de Souza** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)
* **Denis Rosario** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)
* **Christoph Sommer** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)
* **Leandro A. Villas** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)


See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc


conan install . --build missing -s compiler.version=10 -s compiler.libcxx=libstdc++11

PROTO_IN_PATH=/home/jordan/git/Flexe2.0.0/
PROTO_FILE=/home/jordan/git/Flexe2.0.0/flexe.proto
PROTO_OUT_PATH=/home/jordan/git/Flexe2.0.0/src/flexe/proto/

