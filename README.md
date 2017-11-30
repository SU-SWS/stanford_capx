# Stanford CAPx
#### Version 8.x-1.0-dev

CAPx allows users to pull data from cap.stanford.edu in to a Drupal entity.

## What is CAP?

CAP Network is a virtual workspace, originally created by the School of Medicine, to support collaboration among faculty, graduate students, postdocs and staff. In 2013, it was expanded in partnership with various Schools, Institutes, and administrative offices to create the Stanford Profiles website.

Combining a profile directory with a social networking backend, CAP makes it easy for you to work closely with colleagues and track the projects that matter most to you—all in a private, secure environment

* [profiles.stanford.edu](https://profiles.stanford.edu)
* [cap.stanford.edu](https://cap.stanford.edu/)

Accessibility
---
[![WCAG Conformance 2.0 AA Badge](https://www.w3.org/WAI/wcag2AA-blue.png)](https://www.w3.org/TR/WCAG20/)
Evaluation Date: 201X-XX-XX  
This module conforms to level AA WCAG 2.0 standards as required by the university's accessibility policy. For more information on the policy please visit: [https://ucomm.stanford.edu/policies/accessibility-policy.html](https://ucomm.stanford.edu/policies/accessibility-policy.html).

## Installation

Install this module like [any other Drupal module](https://www.drupal.org/documentation/install/modules-themes/modules-8).

## Authentication

Before you get started you will need to have authentication credentials. To get authentication credentials, [file a HelpSU request](https://helpsu.stanford.edu/helpsu/3.0/auth/helpsu-form?pcat=CAP_API&dtemplate=CAP-OAuth-Info) to Administrative Applications/CAP Stanford Profiles.

## Contribution / Collaboration

You are welcome to contribute functionality, bug fixes, or documentation to this module. If you would like to suggest a fix or new functionality you may add a new issue to the GitHub issue queue or you may fork this repository and submit a pull request. For more help please see [GitHub's article on fork, branch, and pull requests](https://help.github.com/articles/using-pull-requests)

### Security
#### HTTPS
CAPx uses https for all API calls. Please follow this best practice as you develop with this module.
#### httpoxy mitigation:
In July 2016, the httpoxy security exploit was announced for PHP, including libraries such as Guzzle. CAPx installs were by default protected because of https usage (see above). In addition, **developers are encouraged to seek their own httpoxy mitigation steps at the server level**. Check with your hosting provider to ensure that your implementation is protected from httpoxy. See https://httpoxy.org for details.
