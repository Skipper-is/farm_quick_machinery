<?php

function farm_quick_machinery_fault_form($form, &$form_state){
  $form['form'] = array(
    '#type'=>'fieldset',
    '#title' => t('Machinery Fault Reporting'),
    '#description' => t('Use this form to report any faults with machines'),
  );

    $form['form']['timestamp'] = array(
    '#type' => 'date_select',
    '#title' => t('Date'),
    '#date_format' => 'M j Y H:i',
    '#date_type' => DATE_FORMAT_UNIX,
    '#date_year_range' => '-10:+3',
    '#default_value' => REQUEST_TIME,
    '#required' => TRUE,
    );

    $form['form']['asset'] = array(
    '#type'=>'textfield',
    '#title' => t('Equipment'),
    '#description' => t('Select the equipment the log is for'),
    '#autocomplete_path' => 'farm_asset/autocomplete/equipment',
    '#required' => TRUE,

      );

    $form['form']['notes'] = array(
      '#type' => 'text_format',
      '#title' => t('Notes'),
      '#format' => 'farm_format',
      '#description' => t('What went wrong? What needs fixing?'),
    );
    $file_directory = $config['settings']['file_directory'];

    // Load supported image file extensions.
    $extensions = farm_fields_file_types('image');

    // Build an upload location path.
    $scheme = variable_get('file_default_scheme', 'public');
    $upload_location = $scheme . '://' . $file_directory;

    $form['form']['image']= array(
      '#type' => 'managed_file',
      '#title' => t('Photos'),
      '#description' => t('Optionally attach a photo to this fault log. If multiple photos need to be attached, edit the log that is created by this form.'),
      '#name' => 'files[]',
      '#progress_indicator' => 'bar',
      '#upload_validators' => array(
        'file_validate_extensions' => array($extensions),
      ),
      '#upload_location' => $upload_location,
    );

    $flag_options = farm_flags_field_allowed_values();

  $form['form']['flags'] = array(
  '#type' => 'checkboxes',
  '#title' => t('Flags<br>'),
  '#options' => $flag_options,
  '#multiple' => TRUE,
  '#prefix' => '<div class="container-inline">',
  '#suffix' => '</div><br>'

  );
  $form['form']['done'] = array(
    '#type'=> 'checkbox',
    '#title' => t('Fixed/Done'),
    '#description' => t('Mark the fault as fixed (Maintenance log as Done)'),
    '#default_value' =>FALSE,
  );




    $form['form']['submit'] = array(
  	'#type' => 'submit',
  	'#value' => t('Save Usage Record'),
    '#suffix' => '<br>',
  	);

  return $form;
}

function farm_quick_machinery_fault_form_validate($form, &$form_state){
  if (!empty($form_state['values']['asset'])){
  $id = 0;
  $matches = array();
  $result = preg_match('/\\[id: ([0-9]+)\\]/', $form_state['values']['asset'], $matches);
  if (!empty($matches[$result])){
    $id=$matches[$result];
  }
if (empty($id)) {
    form_set_error('asset', t('Could not load the equipment record. Make sure the asset ID is included. For example: "Mower [id: 123]"'));
  }

  // Load the asset.
  $asset = farm_asset_load($id);

  // If the asset didn't load, throw an error.
  if (empty($asset)) {
    form_set_error('asset', t('Could not load the equipment. Make sure the asset name and ID are correct.'));
  }

  // Save the asset to the form state.
  $form_state['storage']['asset'] = $asset;
};
}

function farm_quick_machinery_fault_form_submit($form, &$form_state){
  $timestamp
  $image_file_id = $form_state['form']['image'];

  // With the wrapper
      // $file = file_load($image_file_id);
      //
      //
      //   // Make the storage of the file permanent.
      //   $file->status = FILE_STATUS_PERMANENT;
      //
      //   // Save the file information.
      //   file_save($file);
      //
      //   // Save the image with the log.
      //   $log_wrapper->field_farm_images[] = array('fid' => $image_file_id);
}
