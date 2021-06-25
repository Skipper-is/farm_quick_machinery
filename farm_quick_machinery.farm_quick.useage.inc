<?php

function farm_quick_machinery_form($form, &$form_state){
  $form['quick_machinery'] = array(
    '#type'=>'fieldset',
    '#title' => t('Record machinery logs'),
    '#description' => t('Use this form to record machinery logs against equipment'),
  );


  $form['quick_machinery']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Log Name'),
    '#description' => t('Briefly, what you did? eg "Strimmed in Orchard"'),
    '#required' => TRUE,

  );

  $form['quick_machinery']['timestamp'] = array(
  '#type' => 'date_select',
  '#title' => t('Date'),
  '#date_format' => 'M j Y H:i',
  '#date_type' => DATE_FORMAT_UNIX,
  '#date_year_range' => '-10:+3',
  '#default_value' => REQUEST_TIME,
  '#required' => TRUE,
  );

  $form['quick_machinery']['asset'] = array(
  '#type'=>'textfield',
  '#title' => t('Equipment'),
  '#description' => t('Select the equipment the log is for'),
  '#autocomplete_path' => 'farm_asset/autocomplete/equipment',
  '#required' => TRUE,

    );

  $form['quick_machinery']['area'] = array(
    '#type' => 'textfield',
    '#title' => t('Work Area'),
    '#description' => t('Enter the name of the area you worked in, and select it from the list that drops down'),
    '#autocomplete_path' => 'taxonomy/autocomplete/field_farm_area',
    '#required' => TRUE,
  );


  $form['quick_machinery']['description'] = array(
    '#type' => 'text_format',
    '#title' => t('Notes'),
    '#format' => 'farm_format',
    '#description' => t('Suggested things to record in the notes: <ul><li>Usage - Where it was used, what you did</li><li>Refuelled or oiled?</li><li>if any parts have been replaced</li></ul>'),
  );


  $form['quick_machinery']['hours'] = array(
    '#type' => 'textfield',
    '#title' => t('Usage Hours'),
    '#description' => t('Hours on the hour meter on the machine, or the amount of time used for (Please record as decimal hours, such as 1.5h for 1:30)'),
    '#element_validate' => array('element_validate_number'),

  );

  $form['quick_machinery']['faultcheck'] = array(
    '#type'=> 'checkbox',
    '#title' => t('Faults found?'),
    '#description' => t('Any issues identified with the machine? If yes, please list them below'),
    '#default_value'=>FALSE,
    '#ajax'=>array(
     'callback'=>'farm_quick_machinery_quick_form_faults_ajax',
     'wrapper'=>'machinery-faults'
    ),
  );


  $form['quick_machinery']['fault'] = array(
    '#type'=>'container',
    '#prefix' => "<div id='machinery-faults'>",
    '#suffix'=>'</div>',
  );

$checkbox = !empty($form_state['values']['faultcheck']) ? $form_state['values']['faultcheck'] : 0;


if (!empty($checkbox)){

  $form['quick_machinery']['fault']['faultform']= array(
    '#type'=> 'fieldset',
    '#title'=> t('Fault reporting'),
    '#description' => t('Report any faults with equipment here'),

  );
  $form['quick_machinery']['fault']['faultform']['faultnotes'] = array(
    '#type'=> 'text_format',
    '#title' => t('Fault details'),
    '#format' => 'farm_format',
    '#description'=>t('Please enter details of the fault, including where it happened and how it happened.')
  );
  $weekFromNow = format_date(REQUEST_TIME + 604800, 'custom', 'Y-m-d H:i:s');

  $form['quick_machinery']['fault']['faultform']['scheduleDate']= array(
    '#type' => 'date_select',
    '#title' => t('Fix By Date'),
    '#description' => t('By default, this is 7 days from today'),
    '#date_format' => 'M j Y H:i',
    '#date_type' => DATE_FORMAT_UNIX,
    '#date_year_range' => '-10:+3',
    '#default_value' => $weekFromNow,
    '#required' => TRUE,
  );
  $flag_options = farm_flags_field_allowed_values();

$form['quick_machinery']['fault']['faultform']['flags'] = array(
'#type' => 'checkboxes',
'#title' => t('Flags<br>'),
'#options' => $flag_options,
'#multiple' => TRUE,
'#prefix' => '<div class="container-inline">',
'#suffix' => '</div><br>'

);
$form['quick_machinery']['fault']['faultform']['done'] = array(
  '#type'=> 'checkbox',
  '#title' => t('Fixed/Done'),
  '#description' => t('Mark the fault as fixed (Maintenance log as Done)'),
  '#default_value' =>FALSE,
);


  };


  $form['quick_machinery']['submit'] = array(
	'#type' => 'submit',
	'#value' => t('Save Usage Record'),
  '#suffix' => '<br>',
	);


  return $form;
}

function farm_quick_machinery_quick_form_faults_ajax($form, &$form_state){

  return $form['quick_machinery']['fault'];
}

function farm_quick_machinery_form_validate($form, &$form_state){
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

function farm_quick_machinery_form_submit($form, &$form_state){
  $log_type = "farm_activity";
  $log_name = $form_state['values']['title'];
  $timestamp = strtotime($form_state['values']['timestamp']);
  $asset = $form_state['storage']['asset'];
  $assets = array($asset);
  $notes = $form_state['values']['description']['value'];
  $area_name = $form_state['values']['area'];
  $areas = farm_term_parse_names($area_name, 'farm_areas', TRUE);
  if (empty($areas)) {
    drupal_set_message(t('An error occurred while creating/loading areas.'), 'error');
    return;
  };
  $hours = $form_state['values']['hours'];
  $faultcheck = $form_state['values']['faultcheck'];
  if (!empty($faultcheck)){

    $faultnotes = $form_state['values']['faultnotes']['value'];
    $faultnotes = "" . $faultnotes;
    $faulttimstmp = strtotime($form_state['values']['scheduleDate']);
    $flags = array();
    if (!empty($form_state['values']['flags'])){
      foreach($form_state['values']['flags'] as $flag){
        if (strlen($flag) > 1 ){
          $flags[] = $flag;
          };
        };
    };

      $fault_flags = $form_state['values']['flags'];
      $fault_fixed = $form_state['values']['done'];
      $entity_label = entity_label('farm_asset', $asset);
      $fault_name = "Fault with " . $entity_label;

      //Process the fault log
      $done = FALSE;
      if (!empty($fault_fixed)){
        $done=TRUE;
      };

      $logFault = farm_log_create('farm_maintenance', $fault_name, $faulttimstmp, $done,$assets,$faultnotes,$categories=array('Machinery Usage Logs','Fault Report','Equipment') );
      $entity_wrapper = entity_metadata_wrapper('log',$logFault);

      if (!empty($flags)){
          foreach($flags as $flag){
            $entity_wrapper->field_farm_flags[] = $flag;
            };
          };

      foreach($areas as $area){
        $entity_wrapper->field_farm_area[]=$area;
        };
      $entity_wrapper->save();
      };

      $measurements = NULL;
      if (!empty($hours)){
        $measurements = array(
          array(
            'measure'=> 'time',
            'value'=>$hours,
            'units'=>t('Hours'),
            'label' => 'Usage Hours',
          )
        );
      }

    $log = farm_quantity_log_create($log_type, $log_name, $timestamp, TRUE, $assets, $measurements, $notes,$categories=array('Machinery Usage Logs','Equipment'));
    $log_wrapper = entity_metadata_wrapper('log',$log);
    foreach($areas as $area){
      $log_wrapper->field_farm_area[]=$area;
      };
    $log_wrapper->save();
  };
