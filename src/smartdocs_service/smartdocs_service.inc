<?php

use Apigee\ManagementAPI\APIProxy;
use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Revision;
require_once drupal_get_path('module', 'smartdocs') . "/smartdocs.admin.inc";
require_once drupal_get_path('module', 'smartdocs') . "/batch/smartdocs.render.inc";



function _smartdocs_service_swagger_render($model_uuid, $revision_number) {
    
watchdog( 'YL', "Entering render service.");

    $config = devconnect_default_org_config();
  
    $model = new Model($config);
    $model->load($model_uuid);
  
    $revision = new Revision($config, $model_uuid);
    $revision->load($revision_number);
  
    $method_uuids = array();
    foreach ($revision->getResources() as $resource) {
	foreach ($resource->getMethods() as $method) {
            $method_uuids[$method->getUuid()] = $method->getUuid();
	}
    }


    $options = array('publish' => TRUE);
//    $batch = smartdocs_render($model, $revision, $method_uuids, $options);
    $batch =  smartdocs_render($model, $revision, $method_uuids, $options);

//return $batch;
    batch_set($batch);
//		$batch =& batch_get();
//              $batch['progressive'] = FALSE;
//watchdog("YL>R","h"); 
    batch_process();
//drush_backend_batch_process();
}


function _smartdocs_service_swagger_import( $model, $type ) {
watchdog( 'YL', "Entering Import Action. model: " . $model );
watchdog( 'YL', "Entering Import Action. type: " . $type );

    $file_upload_count = count($_FILES['files']['name']);

    $validators = array( 'file_validate_extensions' => array($type));
    
watchdog( 'YL', "Entering Import Action. count: " . $file_upload_count );
    $destination = sys_get_temp_dir();
watchdog( 'YL', "Entering Import Action. tmp dir: " . $destination );
    

    // inlined code after _node_resource_file_save_upload()/node_resource.inc
    $files = $file_objs = array();
    foreach($_FILES['files']['name'] as $key => $val) {


watchdog( 'YL', "Entering Import Action. tmp dir: " . $key . " : " . $val );
    
        if (!$file = file_save_upload($key, $validators) ) {

watchdog( 'YL', "Entering Import Action. Params: " . $_FILES['files']['name'][$key] );
watchdog( 'YL', "Entering Import Action. Params: " . $_FILES['files']['tmp_name'][$key] );


    	    return services_error(t('Failed to upload file. @upload', array('@upload' => $key)), 406);
        }
    
	if( $file->fid ) {
	    $files[] = array(
		'fid' => $file->fid,
		'uri' => services_resource_uri(array('file', $file->fid)),
		);
	    $file_objs[] = $file;

ob_start();
var_dump($file);
$result = ob_get_clean();

watchdog( 'YL', "File: " . $result);


    file_move( $file, "public://" . $file->filename, FILE_EXISTS_REPLACE );
    
    //
    // process the file
    //
    import_swagger_into_model( $model, "public://". $file->filename, $type );

        }else{
    	    return services_error(t('An unknown error occured'), 500);
    	}
    	
    	
    }
}

/**
 * Creates a new model revision from Swagger file and render&publish nodes.
 *
 * @param string $proxy_name
 *   Name of the model.
 * @param \stdClass $swagger_file

 * @parem string $swagger_type swagger_yaml or swagger_json
    
 *   File object which refers to a valid Swagger json file.
 *
 * @throws \Exception
 */
function import_swagger_into_model($model_name, $swagger_file, $swagger_type) {

watchdog( 'YL: import swagger yaml: ', "Entering render service.");

  $config = devconnect_default_org_config();
  $model = new Model($config);
  $form_state = array();
watchdog( 'YL2: import swagger yaml: ', "Entering render service.");

  $form_state['values'] = array(
    'model_name' => $model_name,
    'type' => 'swagger_' . $swagger_type, 
    'upload_type' => 'url',
    'url' => file_create_url($swagger_file),
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
    //    
    // Import latest swagger.
    //
watchdog( 'YL5: uuid: ', "model uuid: ". $form_state['values']['model_uuid']);
    smartdocs_import_model_submit(array(), $form_state);
watchdog( 'YL6: uuid: ', "after the call ". $form_state['values']['type']);

    // Reload the model to get the latest revision after import.
    try {
      $model->load($model->getUuid());
      $model_array = $model->toArray();
      $revision = new Revision($config, $model->getUuid());


watchdog( 'YL7: revision: ', "revision: " . $model->getLatestRevisionNumber() );

      if ($model->getLatestRevisionNumber() > 0) {
        try {
          $revision->load($model->getLatestRevisionNumber());
          $revision_array = smartdocs_prepare_revision($revision);
          $form_state['values']['revision'] = $revision;
          $form_state['values']['model'] = $model_array;
          $form_state['values']['operation'] = 'render_publish_nodes';
          $form_state['values']['methods'] = array();
          foreach ($revision_array['resources'] as $resource) {
            foreach ($resource['methods'] as $method) {
              $form_state['values']['methods'][$method['id']] = $method['id'];
            }
          }
          $term = NULL;
          $terms = taxonomy_get_term_by_name($model_name, 'smartdocs_models');



watchdog( 'YL8: terms: ', "terms: " . $terms );


          if (empty($terms)) {
            $vocab = taxonomy_vocabulary_machine_name_load('smartdocs_models');
            if (!empty($vocab)) {
              $term = new stdClass();
              $term->vid = $vocab->vid;
              $term->name = $model->getName();
              $term->description = $model->getDescription();
              $term->field_model_display_name[LANGUAGE_NONE][0]['value'] = $model->getDisplayName();
              taxonomy_term_save($term);
            }
            watchdog(__FUNCTION__, 'Term with %term name in Smartdocs models vocabulary has not been existed before, we had to create it.', array('%term' => $model_name), WATCHDOG_WARNING);
          }
          else {
            $term = reset($terms);
          }
watchdog( 'YL9: term: ', "term: " . $term->name );
          if ($term !== NULL) {
watchdog( 'YL10: term: ', "term: " . $term->name );
            $term = entity_metadata_wrapper('taxonomy_term', $term);
            // Store last modified date. (We use created time, because when a
            // new model is created the modifiedTime is 0.)

//
// YL: prudentials??
// watchdog( 'YL10: term: ', "term: " . $term->name );
//            $term->field_api_model_modified_at = $revision->getCreatedTime();
//
//watchdog( 'YL10: term: ', "term: " . $term->name );
//            $term->field_api_proxy_name = $proxy_name;
//watchdog( 'YL10: term: ', "term: " . $term->name );
//            $term->field_swagger_documentation = array('fid' => $swagger_file->fid, 'display' => 1);
watchdog( 'YL10: term: ', "term: " . $term->name );
            field_attach_update('taxonomy_term', $term->value());

watchdog( 'YL11: term: ', "term: " . $term->name );


            // Render and publish smartdocs nodes in batch.
            // This executes a batch externally, so there is no way
            // to return within this function.
            smartdocs_revision_detail_submit(array(), $form_state);
            // If cron executes this script we have to process the
            // smartdocs_render_nodes() batch operation with Drush.
            $batch = &batch_get();
            if (drupal_is_cli()) {
              if (function_exists('drush_backend_batch_process')) {
                $batch['progressive'] = FALSE;
                // Use custom error handler to prevent reporting of false
                // positive errors when drush processing smartdocs
                // batch operations.
                // @link https://github.com/drush-ops/drush/issues/1930
                $original_handler = set_error_handler(function () {
                }, E_STRICT | E_NOTICE);
                drush_backend_batch_process();
                // Fix hanging batch processing by Drush.
                $batch = NULL;
                // Restore original error handler.
                set_error_handler($original_handler);
                watchdog(__FUNCTION__, 'Smartdocs methods of %model API Model successfully rendered.', array('%model' => $model_name), WATCHDOG_INFO);
              }
              else {
                watchdog(__FUNCTION__, 'Smartdocs methods %model API Model cannot be rendered, because drush_backend_batch_process() function does not exist.', array('%model' => $model_name), WATCHDOG_WARNING);
              }
            }
            // Workaround for Queue UI vs Cron UI execution.
            // Of course, this also applied for any non Queue UI
            // execution from UI.
            elseif (!isset($batch['form_state']['complete form']['#form_id']) || $batch['form_state']['complete form']['#form_id'] != 'queue_ui_page') {
              $batch = reset($batch['sets']);
              // We need to process it this way to let other functions
              // like _prudential_api_sync_cu_queue_worker() to
              // execute tasks after the rendering has been finished.
              $batch['progressive'] = FALSE;
              batch_set($batch);
              $dest = drupal_get_destination();
              batch_process($dest['destination']);
            }
          }
          else {
            throw new Exception(format_string('Related "@term" term in Smartdocs models does not exist and it cannot be created.', array('@term' => $model->getName())));
          }
        }
        catch (Exception $e) {
          throw $e;
        }
      }
    }
    catch (Exception $e) {
      throw $e;
    }
  }
}

?>
