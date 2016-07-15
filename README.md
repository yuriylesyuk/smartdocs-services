# smartdocs-services
The REST API services module for key functionality to enable CI/CD for swagger-devportal workflow.

The project is a distributed hackaton amongst YL, DC, and GK to come up with a PoC 
for swagger-based CI set of processes. 

Right now we are dealing with a mix of problems that deny smooth out-of-the-box 
experince.

Amongst them:
- bugs in swagger import process (default number type values is not passed
 to a smartdocs json model, Edge java code)

- bugs in SmartDocs rendered theme (model.js; default values are not rendered
 in a sample datagram, /v6/model.js in drupal theme)

- Current DevPortal/SmartDoc solution is flowed and neglected. However, 
the principle and components are right and all the correct moving parts are in place.

As the ultimate goal is a Look & Feel, we can augment current process 
to overcome its implementation limitations.

![solution](docs/smartdocs-ci-solution.png)

Deliverables:

* process for off-side development; 
* on-site update; (ie, 'curl'-based script)
* expose render/publish interface for smardocs via drupal rest module
* new advanced to out-of-the-box sub-theme better fitted for real life
* swagger-driven CI workflow

Features of an alternative Apigee out-of-the-box Theme:
- api page: amadeus left side https://sandbox.amadeus.com/travel-innovation-sandbox/apis/get/location/%7Bcode%7D

- api page: payeasy center and right sides
http://dev-payeezy.devportal.apigee.com/payeezy-api/apis/post/transactions-11

- more options for advanced branding customization; 
http://docs.apigee.com/developer-services/content/using-smartdocs-document-apis

## Misc

Default Values fix: 
https://community.apigee.com/articles/26982/smartdocs-default-values-from-request-body-schema.html




