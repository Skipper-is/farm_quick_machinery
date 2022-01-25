<?php

function farm_quick_machinery_prestart_form($form, &$form_state){
    $form['quick_prestart'] = array(
      '#type' => 'fieldset',
      '#title' => t('Machinery Prestart Checks'),
    );

    $form['quick_prestart']['timestamp'] = array(
        '#type' => 'date_select',
        '#title' => t('Date'),
        '#default_value' => REQUEST_TIME,
        '#date_type' => DATE_FORMAT_UNIX,
        '#date_format' => 'Y-m-d H:i',
        '#date_year_range' => '-10:+3',
        '#required' => TRUE,
    );

    $form['quick_prestart']['asset'] = array(
        '#type' => 'textfield',
        '#title' => t('Equipment'),
        '#required' => TRUE,
        '#description' => t('Enter the equipment the prestart check is for'),
        '#autocomplete_path' => 'farm_asset/autocomplete/equipment',

    );

    $form['quick_prestart']['prestart_oil'] = array(
        '#type' => 'checkbox',
        '#title' => t('Oil'),
        '#default_value' => 0,
        '#description' => t('Is the oil level ok?')
    );

    $form['quick_prestart']['prestart_coolant'] = array(
        '#type' => 'checkbox',
        '#title' => t('Coolant'),
        '#default_value' => 0,
        '#description' => t('Is the coolant level ok?')
    );

    $form['quick_prestart']['prestart_battery'] = array(
        '#type' => 'checkbox',
        '#title' => t('Battery'),
        '#default_value' => 0,
        '#description' => t('Is the battery level ok?')
    );

    $form['quick_prestart']['prestart_fuel'] = array(
        '#type' => 'checkbox',
        '#title' => t('Fuel'),
        '#default_value' => 0,
        '#description' => t('Is the fuel level ok?')
    );

    $form['quick_prestart']['prestart_tires'] = array(
        '#type' => 'checkbox',
        '#title' => t('Tires'),
        '#default_value' => 0,
        '#description' => t('Are the tires ok?')
    );

    $form['quick_prestart']['prestart_brakes'] = array(
        '#type' => 'checkbox',
        '#title' => t('Brakes'),
        '#default_value' => 0,
        '#description' => t('Are the brakes ok?')
    );

    $form['quick_prestart']['prestart_hoses'] = array(
        '#type' => 'checkbox',
        '#title' => t('Hydraulic Hoses'),
        '#default_value' => 0,
        '#description' => t('Are the hydraulic hoses ok?')
    );

    $form['quick_prestart']['prestart_pto'] = array(
        '#type' => 'checkbox',
        '#title' => t('PTO'),
        '#default_value' => 0,
        '#description' => t('Is the PTO ok?')
    );

    $form['quick_prestart']['prestart_hours'] = array(
        '#type' => 'textfield',
        '#title' => t('Hours'),
        '#default_value' => 0,
        '#description' => t('How many hours on the clock before starting?'),
        '#element_validate' => array('element_validate_number'),
    );

    $form['quick_prestart']['prestart_faultcheck'] = array(
        '#type' => 'checkbox',
        '#title' => t('Fault Check'),
        '#default_value' => 0,
        '#description' => t('Is the fault check ok?'),
        '#ajax' => array(
            'callback' => 'farm_quick_machinery_prestart_faultcheck_ajax',
            'wrapper' => 'faultcheck-wrapper',
        ),
    );

    $form['quick_prestart']['faultcheck_wrapper'] = array(
        '#type' => 'container',
        '#prefix' => '<div id="faultcheck-wrapper">',
        '#suffix' => '</div>',
    );

    $form['quick_prestart']['faultcheck_wrapper']['faultform'] = array(
        '#type' => 'fieldset',
        '#title' => t('Fault Check'),
        '#states' => array(
            'visible' => array(
                ':input[name="prestart_faultcheck"]' => array('checked' => TRUE),
            ),
        ),
    );

    $form['quick_prestart']['faultcheck_wrapper']['faultform']['fault_description'] = array(
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#default_value' => '',
        '#description' => t('Enter a description of the fault'),
    );

    $form['quick_prestart']['faultcheck_wrapper']['faultform']['fault_corrective'] = array(
        '#type' => 'textarea',
        '#title' => t('Corrective Action'),
        '#default_value' => '',
        '#description' => t('Enter the corrective action'),
    );

    $form['quick_prestart']['faultcheck_wrapper']['faultform']['scheduleDate'] = array(
        '#type' => 'date_select',
        '#title' => t('Schedule Date'),
        '#default_value' => REQUEST_TIME,
        '#date_type' => DATE_FORMAT_UNIX,
        '#date_format' => 'Y-m-d H:i',
        '#date_year_range' => '-10:+3',
        '#required' => TRUE,
    );

    $flag_options = farm_flags_field_allowed_values();

    $form['quick_prestart']['faultcheck_wrapper']['faultform']['fault_flags'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Flags'),
        '#options' => $flag_options,
        '#multiple' => TRUE,
        '#prefix' => '<div class="container-inline">',
        '#suffix' => '</div><br>',
    );

    $form['quick_prestart']['faultcheck_wrapper']['faultform']['fault_done'] = array(
        '#type' => 'checkbox',
        '#title' => t('Done'),
        '#default_value' => 0,
        '#description' => t('Is the fault fixed?'),
    );

    $form['quick_presart']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
    );

    return $form;

}

function farm_quick_machinery_prestart_faultcheck_ajax($form, &$form_state){
   return $form['quick_prestart']['faultcheck_wrapper'];
}

function farm_quick_machinery_prestart_form_validate($form, &$form_state){
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
        form_set_error('quick_machinery] [asset',t('Could not load the asset. Make sure the asset ID is included. For example "Mower [id:123]"'));

        }
        $asset = farm_asset_load($id);
        if(!empty($asset)){
        $assets[$id] = $asset;
        }
        if(empty($asset)){
        form_set_error('quick_machinery] [asset', t('Could not load the equipment. Make sure the asset name and ID are correct.'));
        }
    }
    $form_state['storage']['assets'] = $assets;

}

function farm_quick_machinery_prestart_form_submit($form, &$form_state){
    $assets = $form_state['storage']['assets'];
    $timestamp = $form_state['values']['timestamp'];
    $asset = reset($assets);
    $asset_name = entity_label('farm_asset', $asset);
    $fault_name = t("Fault with @asset", array('@asset' => $asset_name));
    $log_name = t("Prestart Check for @asset", array('@asset' => $asset_name));
    $hours = $form_state['values']['prestart_hours'];

    $prestart_oil = $form_state['values']['prestart_oil'];
    $prestart_pto = $form_state['values']['prestart_pto'];
    $prestart_coolant = $form_state['values']['prestart_coolant'];
    $prestart_fuel = $form_state['values']['prestart_fuel'];
    $prestart_battery = $form_state['values']['prestart_battery'];
    $prestart_tires = $form_state['values']['prestart_tires'];
    $prestart_brakes = $form_state['values']['prestart_brakes'];
    $prestart_hoses = $form_state['values']['prestart_hoses'];

    $notes = "Prestart Checks\r\n";
    $notes .= "Hours: $hours\r\n";

    $prestart_checks = array(
        'oil' => $prestart_oil,
        'coolant' => $prestart_coolant,
        'fuel' => $prestart_fuel,
        'battery' => $prestart_battery,
        'tires' => $prestart_tires,
        'brakes' => $prestart_brakes,
        'pto' => $prestart_pto,
        'hoses' => $prestart_hoses,
    );

    foreach($prestart_checks as $check => $value){
        if (!empty($value)){
            $notes .= "\r\n" . ucfirst($check) . ": &check;" ;
        }
        else{
            $notes .= "\r\n" . ucfirst($check) . ": &cross;" ;
        }
    }

    $faultcheck = $form_state['values']['prestart_faultcheck'];
    $fault_description = $form_state['values']['fault_description'];
    $fault_corrective = $form_state['values']['fault_corrective'];
    $fault_flags = $form_state['values']['fault_flags'];
    $fault_done = $form_state['values']['fault_done'];
    $scheduleDate = strtotime($form_state['values']['scheduleDate']);

    if (!empty($faultcheck)){
        $flags = array();
        if (!empty($fault_flags)){
            foreach($fault_flags as $flag){
                if (strlen($flag) > 1){
                    $flags[] = $flag;
                };
            };
        };
    $fault_notes = "Fault Description: " . $fault_description . " \r\n Corrective Action: " . $fault_corrective;

    $done = FALSE;
    if (!empty($fault_done)){
        $done = TRUE;
    };

    $fault = farm_log_create('farm_maintenance', $fault_name, $scheduleDate, $done, $assets, $fault_notes, $categories=array(
        'Machinery Usage Logs','Fault Reporting', 'Equipment'));
    $fault_wrapper = entity_metadata_wrapper('log', $fault);

    if (!empty($flags)){
      foreach ($flags as $flag){
          $fault_wrapper->field_farm_flags[] = $flag;
      };
    };
    $fault_wrapper->save();
    }

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

    $log = farm_quantity_log_create("farm_maintenance", $log_name, $timestamp, TRUE, $assets, $measurements, $notes, $categories=array(
        'Machinery Usage Logs','Prestart Checks', 'Equipment'));

}
