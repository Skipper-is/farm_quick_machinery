<?php
function farm_quick_machinery_farm_quick_forms(){
  return array(
    'quick_useage'=> array(
      'label' => t('Machinery Usage Log'),
      'permission' => ' create farm_maintenance log entities',
      'form' => 'farm_quick_machinery_form',
      'file' => 'farm_quick_machinery.farm_quick.useage.inc',
    ),
    'quick_fault'=>array(
      'label'=>t("Machinery Fault Reporting"),
      'permission'=> 'create farm_maintenance log entities',
      'form'=> 'farm_quick_machinery_fault_form',
      'file'=> 'farm_quick_machinery.farm_quick.fault.inc',
    )
  );
};
