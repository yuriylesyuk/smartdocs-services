This module is supposed to expose the CRUD operations for SmartDocs as APIs

This module will allow customers the ability to automate the generation of their Docs.

The module exposes a new role "smartdocs service role" which has the ability to call these APIs.

It is recommended that customers create a new service account and assign this role. This restricts access to what this
account can do.

Please download the module dependencies and place them in the sites/all/modules/contrib folder

Enabling this module will expose an API call on http://developer.client.com/smartdocs/apis/model endpoint


Here are the CURL commands for all operations supported by this module :
Note : Use Basic Auth for these APIs (Username/password is the credential for the service account setup on DevPortal)

1. Get a List of all Models
curl -X GET -H "Authorization: Basic BASE64_ENCODED_USERNAME_PASSWORD"  -H "Accept: application/json" http://portal.loc/smartdocs/apis/models

2. Create a new model with name "abcdE"
curl -X POST -H "Authorization: Basic BASE64_ENCODED_USERNAME_PASSWORD" -H "Content-Type: multipart/form-data;" -F "description=First 5 characters of the alphabet" -F "name=abcdE" -F "display_name=ABCDE" "http://portal.loc/smartdocs/apis/models"

3. Get a model with the name "abcdE"
curl -X GET -H "Authorization: Basic BASE64_ENCODED_USERNAME_PASSWORD" "http://portal.loc/smartdocs/apis/models/abcdE"

4. Delete a Model with the name "abcdE"
curl -X DELETE -H "Authorization: Basic BASE64_ENCODED_USERNAME_PASSWORD" "http://portal.loc/smartdocs/apis/models/abcdE"

5. Import a Swagger(json or yaml)/WADL file to the model with name "abcdE"
curl -X POST -H "Authorization: Basic BASE64_ENCODED_USERNAME_PASSWORD" -H "Content-Type: multipart/form-data;" -F "api_definition=@petstore.swagger.yaml" "http://portal.loc/smartdocs/apis/models/abcdE/import"

6. Rendering of a model
This is a two step process :
6.1 Initiate rendering of the model (this will add the items to be rendered to a queue)
curl -X POST -H "Authorization: Basic BASE64_ENCODED_USERNAME_PASSWORD" "http://portal.loc/smartdocs/apis/models/abcdE/render"

6.2 Actual processing of the queue (This is done by either calling the cron job directly by a CURL request or using drush cron command)
If you login into the portal you will get a cron url with token (Configuration > System > Cron)
You may need to run that URL multiple times to process all items (only 30 items are processed at a time)

You can also run "drush queue-run smartdocs_service_queue" command on the portal server to process these items.

7. Check the number of pending items in the Rendering queue
curl -X POST -H "Authorization: Basic BASE64_ENCODED_USERNAME_PASSWORD" "http://portal.loc/smartdocs/apis/models/queue_status"