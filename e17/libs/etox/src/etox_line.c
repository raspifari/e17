#include "Etox_private.h"

#include <ctype.h>
#include <string.h>

/*
 * etox_line_new - create a new line with the specified alignment
 * @align: the alignment of the current line
 *
 * Returns a pointer to the newly allocated line on success, NULL on failure.
 */
Etox_Line *etox_line_new(char align)
{
	Etox_Line *ret;

	ret = (Etox_Line *) calloc(1, sizeof(Etox_Line));
	if (ret) {
		ret->flags |= align;
		ret->length = 1;
	}

	return ret;
}

/*
 * etox_line_free - free the data structures in a line
 * @line: the line that will be freed
 *
 * Returns no value. Frees all of the data tracked by @line as well as @line
 * itself.
 */
void etox_line_free(Etox_Line * line)
{
	Evas_Object *bit;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Free all of the bits on the line.
	 */
	while (line->bits) {
		bit = line->bits->data;
		evas_object_del(bit);
		line->bits = evas_list_remove(line->bits, bit);
	}

	FREE(line);
}

/*
 * etox_line_show - display all of the bits in the selected line
 * @line: the line to be displayed
 *
 * Returns no value. Displays the text on the specified line.
 */
void etox_line_show(Etox_Line * line)
{
	Evas_Object *bit;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Display all of the bits in the line.
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		evas_object_show(bit);
	}
}


/*
 * etox_line_hide - hide all the bits in the selected line
 * @line: the line to hide
 *
 * Returns no value
 */
void etox_line_hide(Etox_Line * line)
{
	Evas_Object *bit;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Hide all the bits in this line
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		evas_object_hide(bit);
	}
}


/*
 * etox_line_append - append a bit to a line
 * @line: the line to append the bit
 * @bit: the bit to append to the line
 *
 * Returns no value. Appends the bit @bit to the line @line and updates
 * display to reflect the change.
 */
void etox_line_append(Etox_Line * line, Evas_Object * bit)
{
	Evas_Coord x, y, w, h;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	/*
	 * Append the text and update necessary fields
	 */
	line->bits = evas_list_append(line->bits, bit);
	evas_object_geometry_get(bit, &x, &y, &w, &h);

	line->w += w;
	if (h > line->h)
		line->h = h;
	line->length += estyle_length(bit);

        etox_selections_update(bit, line);
}

/*
 * etox_line_prepend - prepend a bit to a line
 * @line: the line to prepend the bit
 * @bit: the bit to prepend to the line
 *
 * Returns no value. Prepends the bit @bit to the line @line and updates
 * display to reflect the change.
 */
void etox_line_prepend(Etox_Line * line, Evas_Object * bit)
{
	Evas_Coord x, y, w, h;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	/*
	 * Prepend the text and update necessary fields
	 */
	line->bits = evas_list_prepend(line->bits, bit);
	evas_object_geometry_get(bit, &x, &y, &w, &h);

	line->w += w;
	if (h > line->h)
		line->h = h;
	line->length += estyle_length(bit);

        etox_selections_update(bit, line);
}

/*
 * etox_line_remove - remove a bit from the line
 * @line: the line to remove the bit
 * @bit: the bit to be from @line
 *
 * Removes @bit from @line and updates the appearance of surrounding bits to
 * reflect this change.
 */
void etox_line_remove(Etox_Line * line, Evas_Object * bit)
{
	Evas_Coord w;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	line->bits = evas_list_remove(line->bits, bit);
	line->length -= estyle_length(bit);
	evas_object_geometry_get(bit, NULL, NULL, &w, NULL);
	line->w -= w;

	/*
	 * FIXME: Need to fix-up line minimizing to ensure it doesn't stomp on
	 * selections.
	 * etox_line_minimize(line);
	 */
}

/*
 * etox_line_layout - layout the bits in a line across the etox
 * @line: the line that has the list of bits and bounding geometry
 *
 * Returns no value. Places the bits in @line across the screen and wraps them
 * appropriately around any fixed bits.
 */
void etox_line_layout(Etox_Line * line)
{
	int x;
	Evas_Object *bit;
	Evas_Coord tx, ty, tw, th;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	if (!line->bits)
		return;

	etox_line_print_bits(line);

	/*
	 * Determine the horizontal alignment of the text and set the starting
	 * x coordinate appropriately.
	 */
	if (line->flags & ETOX_ALIGN_LEFT) {
		x = line->et->x;
	} else if (line->flags & ETOX_ALIGN_RIGHT) {
		x = line->et->x + line->et->w - line->w;
	} else {
		x = line->et->x + (line->et->w / 2) - (line->w / 2);
	}

	if ((line->et->context->flags & ETOX_SOFT_WRAP) &&
			(line->x < line->et->x))
		x = line->et->x;

	/*
	 * Determine the veritcal alignment and perform the layout of the
	 * bits.
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		evas_object_geometry_get(bit, &tx, &ty, &tw, &th);
		if (!estyle_fixed(bit)) {

			if (line->h < th)
				line->h = th;

			/*
			 * Adjust the y position based on alignment.
			 */
			if (line->flags & ETOX_ALIGN_TOP)
				ty = line->y;
			else if (line->flags & ETOX_ALIGN_BOTTOM)
				ty = line->y + line->h - th;
			else
				ty = line->y + (line->h / 2) - (th / 2);

			/*
			 * Move the evas object into place.
			 */
			evas_object_move(bit, x, ty);
		}
		else
			printf("Encountered an obstacle!!\n");

		/*
		 * Move horizontally to place the next bit.
		 */
		x += tw;
	}
}

/*
 * etox_line_minimize - reduce the number of bits on a line
 */
void etox_line_minimize(Etox_Line * line)
{
	Evas_Object *bit, *last_bit = NULL;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	l = line->bits;
	if (!l)
		return;

	last_bit = l->data;
	l = l->next;
	while (l) {
		bit = l->data;

		/*
		 * Attempt to merge the bits if possible, remove the second
		 * one if successful.
		 */
		if (estyle_merge(last_bit, bit)) {
			line->bits = evas_list_remove(line->bits, bit);
			l = evas_list_find_list(line->bits, last_bit);
			l = l->next;
		}
		else {
			last_bit = bit;
			l = l->next;
		}
	}
}

/*
 * etox_line_merge_append - merge lines into the first line, empty the second
 * @line1: the destination of the merged lines
 * @line2: the line that will be merged with line1
 *
 * Returns no value. Moves the bits from line2 into line 1.
 */
void etox_line_merge_append(Etox_Line * line1, Etox_Line * line2)
{
	Evas_Object *bit;

	CHECK_PARAM_POINTER("line1", line1);
	CHECK_PARAM_POINTER("line2", line2);

	/*
	 * Move the bits from line2 to line1.
	 */
	while (line2->bits) {
		bit = line2->bits->data;
		line1->bits = evas_list_append(line1->bits, bit);
		line2->bits = evas_list_remove(line2->bits, bit);

        	etox_selections_update(bit, line1);
	}
	/*
	 * Adjust the height, width and length of the merged line.
	 */
	line1->w += line2->w;
	if (line2->h > line1->h)
		line1->h = line2->h;
	line1->length += line2->length;
}

/*
 * etox_line_merge_prepend - merge lines into the second line, empty the first
 * @line1: the destination of the merged lines
 * @line2: the line that will be merged with line1
 *
 * Returns no value. Moves the bits from line2 into line 1.
 */
void etox_line_merge_prepend(Etox_Line * line1, Etox_Line * line2)
{
	Evas_Object *bit;

	CHECK_PARAM_POINTER("line1", line1);
	CHECK_PARAM_POINTER("line2", line2);

	/*
	 * Move the bits from line2 to line1.
	 */
	while (line1->bits) {
		bit = line1->bits->data;
		line2->bits = evas_list_prepend(line2->bits, bit);
		line1->bits = evas_list_remove(line1->bits, bit);
	}
	/*
	 * Adjust the height, width and length of the merged line.
	 */
	line2->w += line1->w;
	if (line1->h > line2->h)
		line2->h = line1->h;
	line2->length += line1->length;
}

/*
 * etox_line_get_text - retrieve the text from a specified line into a buffer
 * @line: the line to retrieve text
 * @buf: the char buffer to store the found text, must have enough space
 *
 * Returns no value. Saves the text from the line @line into the char buffer
 * @buf.
 */
void etox_line_get_text(Etox_Line * line, char *buf)
{
	char *temp;
	Evas_Object *es;
	Evas_List *l;
	int sum = 0;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("buf", buf);

	/*
	 * Examine each bit on the list of bits and cat it's text onto the end
	 * of the buffer. Then append a \n to the buffer at the end of the
	 * line.
	 */
	for (l = line->bits; l; l = l->next) {
		es = l->data;

		sum += estyle_length(es);

		if (estyle_get_type(es) == ETOX_BIT_TYPE_WRAP_MARKER)
		  continue;

		temp = estyle_get_text(es);
		strcat(buf, temp);
		free(temp);
	}
	line->length = sum;

	/*
	 * FIXME: this shouldn't happen if the next line is a wrapped line
	 * Maybe we should just let whoever calls this func add \n's where
	 * necessary. However, we need to be careful about the line length.
	 */
	strcat(buf, "\n");
}

int
etox_line_wrap(Etox *et, Etox_Line *line)
{
	Evas_List *ll;
	Evas_Object *bit = NULL, *marker;
	Evas_Coord x, w, y, h;
	int index = -1;

	/* iterate through the bits to find the one on the border */
	ll = line->bits;
	while (ll) {
		bit = ll->data;

		evas_object_geometry_get(bit, &x, &y, &w, &h);
		if (x + w > et->x + et->w)
			break;

		ll = ll->next;
	}

	/* get the index of the character on the edge */
	if (bit)
		index = estyle_text_at_position(bit, et->x + et->w, y + (h / 2),
				NULL, NULL, NULL, NULL);

	/* Adjust the index to find the actual character we want to wrap. */
	if (index > 0) {
		char *tmp;

		tmp = estyle_get_text(bit);

		/* Back up to some whitespace when necessary */
		if (et->flags & ETOX_BREAK_WORDS) {
			while (index > 0 && !isspace(tmp[index]))
				index--;
		}

		/* don't start a new line with a space */
		while (index < strlen(tmp) && isspace(tmp[index]))
			index++;
		FREE(tmp);
	}

	/* Wrap if we've found a reasonable position */
	if (index >= 0) {
		etox_line_split(line, bit, index);

		ll = evas_list_find_list(et->lines, line);
		ll = ll->next;

		/* create a marker bit. */
		marker = estyle_new(et->evas, et->context->marker.text,
				et->context->marker.style);
		estyle_set_type(marker, ETOX_BIT_TYPE_WRAP_MARKER);
		evas_object_smart_member_add(marker, et->smart_obj);
		evas_object_color_set(marker, et->context->marker.r,
				et->context->marker.g,
				et->context->marker.b,
				et->context->marker.a);
		evas_object_clip_set(marker, et->clip);
		estyle_set_font(marker, et->context->font,
				et->context->font_size);
		evas_object_show(marker);
		if (et->context->marker.placement == ETOX_MARKER_BEGINNING)
			etox_line_prepend(ll->data, marker);
		else
			etox_line_append(line, marker);
	}
	else
		index = 0;

	return index;
}

void
etox_line_split(Etox_Line *line, Evas_Object *bit, int index)
{
	Evas_List *ll;
	Etox_Line *newline;
	Evas_Object *split = NULL;

	ll = evas_list_find_list(line->bits, bit);

	/*
	 * add the newline after the current one
	 */
	newline = etox_line_new(line->flags | ETOX_LINE_WRAPPED);
	newline->et = line->et;
	line->et->lines = evas_list_append_relative(line->et->lines, newline,
			line);

	/*
	 * If the bit starts on the boundary, simply move it to the next line.
	 */
	if (index > 0) {
		if (index < estyle_length(bit)) {
			split = etox_split_bit(line, bit, index);
		}
		ll = ll->next;
	}

	/*
	 * move the remaining bits to the new line
	 */
	while (ll) {
		/*
		 * Immediately move to the next object, as the node in the
		 * list pointed to by ll will get removed.
		 */
		bit = ll->data;
		ll = ll->next;

		etox_line_remove(line, bit);
		etox_line_append(newline, bit);
	}
}

void
etox_line_unwrap(Etox *et, Etox_Line *line)
{
	Evas_List *l, *prevline;
	Evas_Object *marker;

	if (!et->lines)
		return;

	prevline = evas_list_find_list(et->lines, line);

	l = prevline->next;
	while (l) {
		Evas_List *ll;

		line = l->data;
		if (!(line->flags & ETOX_LINE_WRAPPED))
			break;

		/* remove any wrap marker bits */
		ll = line->bits;
		while (ll) {
			marker = ll->data;

			ll = ll->next;

			if (estyle_get_type(marker) == 
			  ETOX_BIT_TYPE_WRAP_MARKER) {
				line->bits = evas_list_remove(line->bits, marker);
			}
		}

		/* remove the line from the list */
		et->lines = evas_list_remove(et->lines, line);

		/* merge the two lines */
		etox_line_merge_append(prevline->data, line);
		etox_line_free(line);

		l = prevline->next;
	}
        
//        etox_line_minimize(line);
}

Evas_Object *
etox_line_coord_to_bit(Etox_Line *line, int x)
{
	Evas_Coord bx;
	Evas_List *l = NULL;
	Evas_Object *bit = NULL;

	/*
	 * Find the bit on this line
	 */
	l = line->bits;
	while (l) {
		bit = l->data;
		evas_object_geometry_get(bit, &bx, NULL, NULL, NULL);
		if (bx < x)
			break;
		l = l->next;
	}

	return bit;
}

Evas_Object *
etox_line_index_to_bit(Etox_Line *line, int *i)
{
	int len = 0;
	Evas_List *l = NULL;
	Evas_Object *bit = NULL;

	l = line->bits;
	while (l) {
		bit = l->data;
		len += estyle_length(bit);
		if (*i < len)
			break;
		l = l->next;
	}

	if (l)
		*i -= (len - estyle_length(bit));

	return bit;
}

void
etox_line_print_bits(Etox_Line *line)
{
	int i = 0;
	Evas_List *l;

	for (l = line->bits; l; l = l->next) {
		printf("\tBit %d: %s\n", i, estyle_get_text(l->data));
		i++;
	}
}

void
etox_line_set_layer(Etox_Line *line, int layer)
{
  Evas_List *l;

  if (!line->bits) return;
  
  for (l = line->bits; l; l = l->next)
  {
    Evas_Object *bit;

    bit = l->data;
    evas_object_layer_set(bit, layer);
  }
}

void
etox_line_index_to_geometry(Etox_Line *line, int index, Evas_Coord *x,
			    Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
  Evas_Object *bit = NULL;
  Evas_List *l;
  int sum = 0;

  /* find the bit containing the character */
  for (l = line->bits; l; l = l->next)
  {
    int length;
    
    bit = l->data;
    length = estyle_length(bit);

    if ( sum + length < index)
      break;

    sum += length;

    if (!l->next)
      bit = NULL;
  }

  /* No bit intersects, so set the geometry to the end of the
   * line, with the average character width on the line
   */
  if (!bit)
  {
    if (h) *h = line->h;
    if (w) *w = line->w / line->length;
    if (y) *y = line->y;
    if (x) *x = line->x + line->w;
    return;
  }

  /* get the geometry from the bit */
  estyle_text_at(bit, index - sum, x, y, w, h); 
}

void
etox_line_apply_context(Etox_Line *line, Etox_Context *context, Evas_Object *start, Evas_Object *end)
{
  Evas_List *l, *ls = NULL, *le = NULL;

  ls = evas_list_find_list(line->bits, start);
  le = evas_list_find_list(line->bits, end);
  
  /* make sure start and end exist and are in line->bits */
  if ( !ls )
    ls = line->bits;
  if ( !le ) 
    le = evas_list_last(line->bits); 

  for (l = ls; l; l = l->next)
  {
    Evas_Object *bit;

    bit = l->data;

    if (!l->prev && line->flags & ETOX_LINE_WRAPPED)
    {
      /* go past any obstacles */
      while (estyle_fixed(bit))
      {
        /* if there are only obstacles on the line (can this happen?) */
        if (!l->next)
          return;

        l = l->next;
        bit = l->data;
      }
      estyle_set_text(bit, context->marker.text);
      estyle_set_style(bit, context->marker.style);
      evas_object_color_set(bit, context->marker.r, context->marker.g,
                           context->marker.b, context->marker.a);
    }
    else
    {
      estyle_set_style(bit, context->style);
      evas_object_color_set(bit, context->r, context->g, context->b,
                            context->a);
      estyle_set_font(bit, context->font, context->font_size);
    }
    if (l == le)
      break;
  }
}

void
etox_line_get_geometry(Etox_Line *line, Evas_Coord *x, Evas_Coord *y,
                       Evas_Coord *w, Evas_Coord *h)
{
  if (!line)
  {
    if (x) *x = 0;
    if (y) *y = 0;
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }
  
  if (x) *x = line->x;
  if (y) *y = line->y;
  if (w) *w = line->w;
  if (h) *h = line->h;
}
