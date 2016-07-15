<?php

use Apigee\ManagementAPI\APIProxy;
use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Revision;
require_once drupal_get_path('module', 'smartdocs') . "/smartdocs.admin.inc";


function _smartdocs_service_swagger_import( $name ) {
    watchdog( 'YL', "Entering render service.");


    import_swagger_yaml_into_model();
    return 'hi';

}

/**
 * Creates a new model revision from Swagger file and render&publish nodes.
 *
 * @param string $proxy_name
 *   Name of the model.
 * @param \stdClass $swagger_file
 *   File object which refers to a valid Swagger json file.
 *
 * @throws \Exception
 */
function import_swagger_yaml_into_model() {

watchdog( 'YL: import swagger yaml: ', "Entering render service.");


  $model_name = "model6";
  $config = devconnect_default_org_config();
  $model = new Model($config);
  $form_state = array();
watchdog( 'YL2: import swagger yaml: ', "Entering render service.");

  $form_state['values'] = array(
    'model_name' => $model_name,
    'type' => 'swagger_yaml',
    'upload_type' => 'url',
    'url' => file_create_url("/tmp/swagger.yaml"),
  );

  // DEBUG only option for local development environments.
  if (variable_get('swagger_local_debug', FALSE)) {
    $form_state['values']['url'] = prudential_smartdocs_file_create_url($swagger_file->uri);
  }

watchdog( 'YL2: import swagger yaml: ', "Entering render service: " . sys_get_temp_dir() . ", " . $form_state['values']['url']);

  $model_array = smartdocs_model_load($form_state['values']['model_name']);

watchdog( 'YL3: import swagger yaml: ', "Entering render service.");

  if (!$model_array) {
    throw new Exception(format_string('Model does not exist in Developer Portal with %name name.', array('%name' => $model_name)));
  }
  else {
watchdog( 'YL4 else: import swagger yaml: ', "Entering render service: ". $form_state['values']['url']);

    Model::fromArray($model, $model_array);
    $form_state['values']['model_uuid'] = $model->getUuid();
    // Import latest swagger.
watchdog( 'YL5: uuid: ', "model uuid: ". $form_state['values']['model_uuid']);
    smartdocs_import_model_submit(array(), $form_state);
watchdog( 'YL6: uuid: ', "after the call ". $form_state['values']['type']);
  }
}


?>
