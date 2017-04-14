<?php
class local__germplasm_summary extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The module that manages this field.
  public static $module = 'trpexp_fields';

  // The default lable for this field.
  public static $default_label = 'Germplasm Summary';

  // The default description for this field.
  public static $description = 'Summarizes germplasm for an organism.';

  // A list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'field_chado_storage',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'germplasm_summary',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'germplasm_summary',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'organism',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'organism_id',
    // The base table.
    'base_table' => 'organism',
  );


  // The default widget for this field.
  public static $default_widget = 'local__germplasm_summary_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__germplasm_summary_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;
  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;

  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   * @param $details
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the enity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($entity, $details = array()) {

    // First, retrieve the chado organism_id for the current Tripal Content, as we requested.
    $organism_id = $entity->local__germplasm_summary['und'][0]['chado-organism__organism_id'];

    // Then grab the information we need from the materialized view: organism_stock_count.
    // This materialized view is provided via tripal_chado :-).
    $results = chado_query(
      'SELECT stock_type, num_stocks FROM {organism_stock_count} WHERE organism_id=:id ORDER BY num_stocks DESC',
      array(':id' => $organism_id));
    $entity->local__germplasm_summary['und'][0]['value'] = array();
    foreach ($results as $r) {
      $entity->local__germplasm_summary['und'][0]['value'][ $r->stock_type ] = $r->num_stocks;
    }
   
  }

}
