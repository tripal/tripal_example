<?php
/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class local__germplasm_summary extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Germplasm Summary';

  // The default description for this field.
  public static $default_description = 'Summarizes the stocks for a given organism.';

  // The default widget for this field.
  public static $default_widget = 'local__germplasm_summary_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__germplasm_summary_formatter';

  // The module that manages this field.
  public static $module = 'trpexp_fields';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'field_chado_storage',
     // It is expected that all fields set a 'value' in the load() function.
     // In many cases, the value may be an associative array of key/value pairs.
     // In order for Tripal to provide context for all data, the keys should
     // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
     // function that are supported by the query() function should be
     // listed here.
     'searchable_keys' => array(),
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'germplasm_summary',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'germplasm_summary',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'organism',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'organism_id',
    // The base table.
    'base_table' => 'organism',
  );

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

 /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
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
   */
  public function load($entity) {
    $field_name = $this->instance['field_name'];

    // ChadoFields automatically load the chado column specified in the
    // default settings above. If that is all you need then you don't even
    // need to implement this function. However, if you need to add any
    // additional data to be used in the display, you should add it here.
    parent::load($entity);

    // First, retrieve the chado organism_id for the current Tripal Content, as we requested.
    $organism_id = $entity->{$field_name}['und'][0]['chado-organism__organism_id'];

    // Then grab the information we need from the materialized view: organism_stock_count.
    // This materialized view is provided via tripal_chado :-).
    $results = chado_query(
      'SELECT stock_type, num_stocks FROM {organism_stock_count} WHERE organism_id=:id ORDER BY num_stocks DESC',
      array(':id' => $organism_id));
    $total_count = 0;
    $entity->{$field_name}['und'][0]['value'] = array();
    $entity->{$field_name}['und'][0]['value']['hydra:member'] = array();
    foreach ($results as $r) {
      $entity->{$field_name}['und'][0]['value']['hydra:member'][] = [
        'rdfs:type' => $r->stock_type,
        'local:count' => $r->num_stocks,
      ];
      $total_count = $total_count + $r->num_stocks;
    }
    $entity->{$field_name}['und'][0]['value']['local:count'] = $total_count;
    dpm($entity->{$field_name}['und'][0], 'load');

  }

  /**
   * @see ChadoField::elementInfo()
   *
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    return array(
      $field_term => array(
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'label' => 'Germplasm Summary',
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => array(
          'local:count' => array(
            'searchable' => TRUE,
            'name' => 'The total number of germplasm for this organism.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'readonly' => TRUE,
            'type' => 'xs:string',
            'required' => FALSE,
          ),
          'hydra:members' => array(
            'searchable' => TRUE,
            'name' => 'Breakdown of germplasm by type.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'readonly' => TRUE,
            'type' => 'xs:string',
            'required' => FALSE,
            'elements' => array(
              'rdfs:type' => array(
                'searchable' => TRUE,
                'name' => 'The type of germplasm.',
                'operations' => ['eq', 'ne', 'contains', 'starts'],
                'sortable' => TRUE,
                'readonly' => TRUE,
                'type' => 'xs:string',
                'required' => FALSE,
              ),
              'local:count' => array(
                'searchable' => TRUE,
                'name' => 'The number of germplasm for this type.',
                'operations' => ['eq', 'ne', 'contains', 'starts'],
                'sortable' => TRUE,
                'readonly' => TRUE,
                'type' => 'xs:string',
                'required' => FALSE,
              ),
            ),
          ),
        ),
      ),
    );
  }
}
