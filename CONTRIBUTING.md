# Contributing to the Common Model Library

The Common Model Library accepts new models and updates to existing models from NASA
simulation-developing groups and any other groups to whom CML provides value. Before
submitting changes, please look at our [coding standards](docs/CML_Coding_Standards.pdf)
in the `docs` folder. If you're using an older version of CML, please also double check
that the changes you're proposing have not already been integrated into CML in a more
recent release.

Proposed contributions should be submitted to CML in the form of a Pull Request in
CML's [core repository](https://github.com/nasa/cml). When your change is ready
for review by the CML team, take your pull request out of draft status. Each pull
request should contain a cohesive, minimal set of changes to the CML codebase. If you
are feeding back multiple unrelated changes to CML, please open multiple pull requests.
This helps us review contributions faster and maintain a coherent Git history.

CML maintains a robust testing pipeline which ensures that modifications to the code
do not break existing functionality. Changes which result in a failed continuous
integration (CI) pipeline will not be accepted under any circumstances.

## Getting Access

You must have a NASA identity to push to this repository. To obtain
contributor privileges, fill out the
[HQ AGCY NASA GitHub Collaborator](http://nams.nasa.gov/nams/asset/227756) request.
For the business justification, state that you need contributor permissions for the
cml repository. After that request is approved, you will have the ability to push
changes to CML.

If you do not have a NASA identity or do not wish to submit a GitHub Collaborator
request, you may still contribute to CML. Create a
[fork](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/fork-a-repo)
of this repository. You may push your changes to a branch inside your forked CML
repository. When your changes are ready for review, you may open a pull request in
your fork which targets the core CML repository. For more information, see GitHub's
[guide](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request-from-a-fork)
to creating pull requests from forked repositories.

## Submitting a Change to an Existing Model

When submitting a change to an existing model, be aware that your changes may impact
other CML-using groups. As such, large API or infrastructure changes may take a
significant time to be approved. If the impact to other CML-using groups is deemed
too great without sufficient rationale, the requested change may be rejected.

Any changes you submit must be accompanied by changes to the model's documentation
that accurately reflect the new state of the model. If you added new lines of code to
a model, you must provide evidence that statement coverage of the model's tests
continue to cover at least 90% of lines in the source code. New functionality must
be covered by at least one test.

## Submitting a New Model

When submitting a new model to CML, you must provide:
- The NASA NPR 7150.2 classification of the model.
- Evidence of NASA-STD-7009 compliance.
- The name of the originating group and a point of contact.

Models must meet NPR 7150.2 Class A, B, C, or D to be accepted into CML. Additionally,
all new models must have unit tests which cover at least 90% of source code lines and
must be submitted alongside documentation which adequately describes the model. See the [coding standards](docs/CML_Coding_Standards.pdf) for more information.

Models must also provide sufficient value to CML to be accepted. Trivial models which
contain only a handful of lines of code can add a large maintenance burden to the CML team
while providing little benefit to the project. Models which are very restrictive in their
usage or make a large number of assumptions may be generic in theory, but in practice
may only be useful for a single program. Lastly, please check whether the model you're
providing is already provided by the C++ standard library or open-source projects such
as [Boost](https://www.boost.org/), [Abseil](https://abseil.io/), and
[Eigen](https://libeigen.gitlab.io/).

Any change to CML that modifies project dependencies may be rejected if that change is not
sufficiently in the interest of the user base. CML dependencies must be trusted, stable,
and contain permissive licensing for distribution.

To reduce the number of reviews by the CML team, we ask that models which are in active
development not be submitted to CML unless the model is urgently needed by another group.
In most cases, models in CML should be in a relatively stable state and should have
undergone prior verification and validation.

### By Submitting Your Model to CML, You Agree to the Following

When you submit a model to CML, you waive any right to object to stylistic changes that
CML may make to better integrate your model into our codebase. These changes may
include, but are not limited to:
- Renaming your model or recategorizing it into a different subdirectory.
- Changing the formatting of your source code.
- Changing the naming convention used for variables, types, and functions.
- Moving some or all of your code into a different namespace.

CML may also make non-breaking changes to your model without consulting you, such as:
- Modifying the unit tests or adding new tests.
- Updating the documentation that you provided.
- Modernizing your code when we upgrade our minimum supported C++ standard.

We ask that you provide an originating group and a point of contact when submitting
your model because we will attempt to consult you when someone outside your group
proposes a change to your model.

## Artificial Intelligence (AI) Policy

The CML team supports the responsible usage of AI assistance for software development.
You may use AI tools when contributing to CML, subject to the following guidelines:
- Pull requests, issues, comments, and any other submission to the CML GitHub
  repository must be authored by a human. AI tools are not permitted to automatically
  open an issue or respond to a pull request review.
- A pull request must be fully understood by the human author who submits it.
- AI tools are not permitted to be credited as authors. An AI tool may not be listed as
  the co-author of a commit, the author of a change to documentation, or an author in
  the Trick header of a source file, for example.

Before marking a pull request as ready for review, please ensure that any AI-generated
code, documentation, and tests adhere to our contribution guidelines and coding
standards.