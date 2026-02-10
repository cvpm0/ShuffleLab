## Overview

ShuffleLab is a performance-focused C++ project for modelling and analysing card-shuffling algorithms, with an emphasis on probabilistic behaviour, empirical randomness testing, and explicit modelling assumptions.

The project supports both idealised mathematical shuffle models and human-inspired shuffling behaviour, providing a framework for running controlled experiments and analysing how different shuffling mechanisms behave relative to uniform random permutations.

## Motivation

Classical results show that common shuffling procedures converge to uniform randomness at very different rates. For example, Bayer and Diaconis famously showed that approximately seven riffle shuffles are required to adequately randomise a standard deck, while other work has demonstrated that overhand shuffles may require on the order of ten thousand repetitions to approach similar randomness.

In practice, such idealised procedures are often infeasible. In live, self-dealt tournament poker, time pressure frequently limits players to fewer than seven shuffles, raising a natural empirical question: how do different shuffling strategies behave when full randomisation is not achievable?

ShuffleLab was developed to investigate this gap between theory and practice by analysing shuffle behaviour under constrained shuffle counts and realistic modelling assumptions.

## Capabilities

ShuffleLab provides a framework for experimentally analysing card-shuffling behaviour under controlled conditions. Core capabilities include:

- Modelling both idealised mathematical shuffle processes and human-inspired shuffling behaviour
- Supporting established shuffle models (e.g. riffle shuffles) alongside custom, distribution-driven models
- Running large numbers of controlled trials under fixed or constrained shuffle counts
- Generating deterministic, reproducible experimental runs via explicit RNG control
- Applying statistical analyses to assess uniformity, entropy, and convergence behaviour
- Reporting results via a lightweight command-line interface focused on analysis rather than presentation


## Design and Architecture

ShuffleLab is designed to make experimental assumptions explicit while keeping the overall execution model inspectable. The system is structured around a small number of conceptual stages, each responsible for a distinct part of the experimental process.

### Design Goals

- **Reproducibility-oriented design**  
  Randomness is explicitly controlled and centralised, with the system structured to support deterministic, repeatable experimental runs.

- **Explicit modelling assumptions**  
  Shuffle behaviour is encoded directly in model definitions, making constraints and biases visible rather than hidden behind generic randomness.

- **Separation of concerns**  
  Shuffle modelling, experiment orchestration, and statistical analysis are decoupled to simplify reasoning and modification.

- **Performance transparency**  
  Computational costs are kept visible, with hot paths designed to minimise unnecessary allocation or abstraction.

### High-Level Architecture

At a conceptual level, the experimental workflow can be viewed as a staged pipeline:

Experiment Configuration -> RNG and Shuffle Model Selection -> Experiment Execution -> Statistical Accumulation -> CLI Reporting

Internally, this pipeline involves multiple intermediate steps and configuration layers, but this abstraction captures the core flow of data through the system.

## Modelling and Randomness

ShuffleLab treats randomness and shuffle behaviour as explicit modelling choices rather than implicit implementation details. Both idealised and human-inspired shuffling mechanisms are represented directly, allowing their assumptions and limitations to be examined experimentally.

### Random Number Generation

The project uses a hand-implemented PCG32 random number generator, integrated directly into the codebase rather than relying on standard library RNG facilities.

This design choice provides fine-grained control over random state, seeding behaviour, and usage patterns, and avoids opaque abstractions that can obscure experimental assumptions. Centralising randomness in this way also supports reproducibility-oriented experimentation and makes the effects of randomness easier to reason about and audit.

### Shuffle Models

ShuffleLab supports multiple approaches to modelling shuffle behaviour:

- **Idealised mathematical models**, such as riffle shuffles based on the Gilbert–Shannon–Reeds (GSR) framework, which provide a theoretical baseline for convergence to uniform randomness.

- **Human-inspired shuffle models**, where behaviour is defined using explicitly specified probability distributions to capture biases and constraints observed in real-world shuffling.

By supporting both categories within the same experimental framework, ShuffleLab enables direct comparison between theoretical expectations and empirically modelled human behaviour under constrained conditions.

### Modelling Philosophy

Rather than attempting to certify shuffles as “sufficiently random”, ShuffleLab focuses on comparative analysis. Shuffle models are evaluated relative to one another and to idealised baselines, with an emphasis on understanding how randomness degrades or improves under different assumptions and constraints.

## Experimental Workflow

ShuffleLab is organised around an experiment-driven workflow, where configuration, randomness, and modelling choices are treated as explicit inputs rather than implicit behaviour.

An experiment proceeds through the following stages:

1. **Experiment configuration**  
   Each run begins with an explicit configuration specifying shuffle models, shuffle counts, trial counts, and analysis options. This configuration defines the experimental constraints under which behaviour is evaluated.

2. **Randomness and model initialisation**  
   A central random number generator is initialised and passed explicitly to shuffle models and experiment logic, ensuring that sources of randomness are visible and controllable.

3. **Trial execution**  
   For each trial, shuffle sequences are applied according to the selected model and constraints. The experiment runner coordinates shuffle application and deck state evolution across trials.

4. **Statistical accumulation**  
   As trials complete, summary statistics are incrementally updated. These statistics are designed to capture aggregate properties of shuffle behaviour rather than individual outcomes.

5. **Reporting**  
   After all trials complete, accumulated statistics are reported via a lightweight command-line interface focused on analytical output rather than presentation.

### Statistical Analysis

ShuffleLab currently applies a small set of exploratory statistical measures to characterise shuffle behaviour, including:

- Position-based frequency analysis to assess deviation from uniformity
- Entropy-based measures to capture distributional spread
- Aggregate convergence indicators tracked across repeated trials

These analyses are intended to support comparative investigation between shuffle models and constraints, rather than to provide formal guarantees of randomness.

## Build and Run

ShuffleLab is built as a native C++ application and is intended to be run from the command line.

### Requirements

- C++17 or newer
- CMake
- A modern C++ compiler (GCC or Clang)

### Build

~~~bash
git clone https://github.com/cvpm0/shufflelab.git
cd shufflelab
mkdir build && cd build
cmake ..
make
~~~

### Run

~~~bash
./shufflelab [options]
~~~

Experiments are configured via explicit runtime options and configuration structures in the codebase. The command-line interface is intentionally minimal and focused on driving experimental runs rather than user-facing presentation.

## Limitations

ShuffleLab is intended as an exploratory analysis tool and makes no claims of providing formal guarantees about randomness or fairness.

- The statistical measures currently implemented are limited in scope and are designed for comparative analysis rather than exhaustive or adversarial testing.
- Human-inspired shuffle models are simplified abstractions and do not attempt to fully capture the complexity or variability of real-world behaviour.
- Experimental reproducibility is a design goal, but full persistence of configuration and random seeds is not yet enforced.
- The command-line interface prioritises analytical output over usability and presentation.

These limitations are intentional and reflect the project’s focus on transparency, interpretability, and iterative experimentation.

## Future Work

Planned and potential extensions to ShuffleLab include:

- **Expanded shuffle models**  
  Adding additional idealised and human-inspired shuffle mechanisms, including more flexible composition of custom shuffle sequences and constraints.

- **Data-informed human modelling**  
  Using raw empirical data (e.g. observed card positions or shuffle traces) to inform and calibrate probability distributions used in human-inspired shuffle models, rather than relying solely on hand-specified assumptions.

- **Custom user specified distributions and parameterisation**  
  Supporting richer, configurable distributions for modelling bias and variability in human shuffling behaviour across different contexts.

- **Parallel experiment execution**  
  Introducing multithreaded execution to accelerate large experimental runs while preserving explicit control over randomness and reproducibility.


## References

- Bayer, D., & Diaconis, P. (1992). Trailing the Dovetail Shuffle to its Lair.
