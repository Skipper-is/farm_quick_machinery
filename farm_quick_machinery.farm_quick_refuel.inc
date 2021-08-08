<?php

function farm_quick_machinery_refuel_form($form, &$form_state){
  $fuel_types = array(
    'diesel' => t("Red Diesel"),
    'petrol4' => t("Petrol (or Aspen 4 stroke)"),
    'petrol2'=> t("2 Stroke Petrol (or Aspen 2 stroke)"),
    );

  $form['form'] = array(
    '#type'=>'fieldset',
    '#title'=> t('Refuel Machinery'),
    '#description'=>t("Use this form when refuelling machinery including tractors and mowers"),
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

  $form['form']['hours'] = array(
  '#type' => 'textfield',
  '#title' => t('Usage Hours'),
  '#description' => t('Hours on the hour meter on the machine, or the amount of time used for (Please record as decimal hours, such as 1.5h for 1:30)'),
  '#element_validate' => array('element_validate_number'),

);


    $form['form']['fuel_type']=array(
      '#type'=>'radios',
      '#title'=>t("Fuel Type"),
      '#options'=>$fuel_types,
      '#description'=>t("Select which type of fuel you refuelled with"),
      '#default_value'=> 'diesel',
      '#required'=>TRUE,
    );

  $form['form']['litres'] = array(
  '#type' => 'textfield',
  '#title' => t('Litres refuelled'),
  '#description' => t('How many litres you put in the tank (Approximate for strimmers and mowers)'),
  '#element_validate' => array('element_validate_number'),
  '#required'=>TRUE,
  );

  $form['form']['tank'] = array(
    '#type' => 'textfield',
    '#title' => t('Red diesel tank total'),
    '#description' => t('If using the red diesel tank, please enter the total litres dispensed (On the smaller display on the tank)'),
    '#element_validate' => array('element_validate_number'),

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


  $form['form']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Refuelling Record'),
    '#suffix' => '<br>',
  );
return $form;
}

function farm_quick_machinery_refuel_form_validate($form, &$form_state){
  $assets = array();
  $asset_names = drupal_explode_tags($form_state['values']['asset']);
  foreach($asset_names as $asset_name){
    $id = 0;
    $matches = array();
    $result =preg_match('/\\[id: ([0-9]+)\\]/', $asset_name, $matches);
    if(!empty($matches[$result])){
      $id = $matches[$result];
    }
    if(empty($id)){
      form_set_error('form] [asset',t('Could not load the asset. Make sure the asset ID is included. For example "Mower [id:123]"'));

    }
    $asset = farm_asset_load($id);
    if(!empty($asset)){
      $assets[$id] = $asset;
    }
    if(empty($asset)){
      form_set_error('form] [asset', t('Could not load the equipment. Make sure the asset name and ID are correct.'));
    }
  }
  $form_state['storage']['assets'] = $assets;

}

function  farm_quick_machinery_refuel_form_submit($form, &$form_state){
  $log_type = "farm_activity";
  $assets = $form_state['storage']['assets'];
  $asset = reset($assets);
  $timestamp = strtotime($form_state['values']['timestamp']);
  $flags = array();
    if (!empty($form_state['values']['flags'])){
      foreach($form_state['values']['flags'] as $flag){
        if (strlen($flag) > 1 ){
          $flags[] = $flag;
          };
        };
    };
  $litres_fuel = $form_state['values']['litres'];

  $entity_label = entity_label('farm_asset', $asset);
  $log_name = t("Refuelled @entity_label with @volume L", array('@entity_label'=> $entity_label, '@volume'=>$litres_fuel));
  $fuel_types = array(
    'diesel' => t("Red Diesel"),
    'petrol4' => t("Petrol (or Aspen 4 stroke)"),
    'petrol2'=> t("2 Stroke Petrol (or Aspen 2 stroke)"),
    );

  $fuel_type_form = $form_state['values']['fuel_type'];
  $fuel_type = $fuel_types[$fuel_type_form];
  $notes = t("Refuelled @entity_label with @volume L of @fuel_type.", array('@entity_label'=>$entity_label, '@volume'=> $litres_fuel, '@fuel_type'=>$fuel_type));
  $hours = $form_state['values']['hours'];
  $tank_litres = $form_state['values']['tank'];

  $measurements = array();

  if (!empty($hours)){
    $measurements[] = array(
      'measure'=>'time',
      'value'=>$hours,
      'units'=>t('Hours'),
      'label'=>t("Usage Hours"),

    );
  };


  if(!empty($litres_fuel)){
    $measurements[] = array(
      'measure'=>'volume',
      'value'=>$litres_fuel,
      'units'=>t('litres'),
      'label'=>t("Litres of @fuel_type",array('@fuel_type'=>$fuel_type)),
    );
  };

  if(!empty($tank_litres)){
    $measurements[] = array(
      'measure'=>'volume',
      'value'=>$tank_litres,
      'units'=>t('litres'),
      'label'=>t("Total litres dispensed"),
    );
  };
$log = farm_quantity_log_create($log_type, $log_name, $timestamp, TRUE, $assets, $measurements, $notes,$categories=array('Machinery Usage Logs','Equipment','Fuel'));
}
