<?php
/**
 * @file
 * This page provides an entrypoint for  developers to learn about the 
 * field examples available.
 */
?>

<p>The Tripal API provides a class-based Tripal Fields API for creating custom fields to be used with Tripal Content Types. For a refresher on the terminology surrounding Tripal Content Types and Fields see <a href="http://tripal.info/node/347">Terminology on Tripal.info</a>.</p>

<p>The Tripal Example: Tripal Fields module provides the following examples to aid developers in creating their own custom Tripal Fields.</p>

<h3>Alter existing field display (Comming Soon!)</h3>
<p>This example shows you how to add a custom display to an existing Tripal/Chado Field. This is done by creating a custom <em>Tripal Field Formatter</em> and making it available to the field you would like to customize the display for. This method replaces the Tripal2 approach of creating custom template files.</p>

<h3>Add new content to Tripal Content Type(s)</h3>
<p>The best way to add new content to a Tripal Content Type(s) is to create a custom field. This example shows you how to add a stock summary to your Tripal Organism pages by:</p>
<ol>
  <li>Create custom <a href="https://github.com/tripal/tripal_example/blob/7.x-3.x/trpexp_fields/includes/TripalFields/local__germplasm_summary/local__germplasm_summary.inc"  target="_blank">Chado Field</a> which pulls the chado organism_id for a given Organism page and then uses that in conjunction with the organism_stock_counts materialized view to retrieve the number of stocks per stock type for the given organism.</li>
  <li>Create custom <a href="https://github.com/tripal/tripal_example/blob/7.x-3.x/trpexp_fields/includes/TripalFields/local__germplasm_summary/local__germplasm_summary_widget.inc" target="_blank">Chado Widget</a>, controls the edit form for this field, that doesn't allow editing of this content.</li>
  <li>Create custom <a href="https://github.com/tripal/tripal_example/blob/7.x-3.x/trpexp_fields/includes/TripalFields/local__germplasm_summary/local__germplasm_summary_formatter.inc" target="_blank">Chado Formatter</a>, controls the display of this field, that output a Drupal Table with the number of stocks per stock type for this organism.</li>
  <li>Make this field available to the Tripal Organism Content Type by implementing hook_bundle_fields_info() and setting <code>public static $no_ui = FALSE;</code> in the field.</li>
  <li>Automatically add this field to Tripal Organism Content Type by implementing hook_bundle_instances_info().</li>
</ol>
