typedef struct point_node_t
{
    Vector2 pos;
    point_node_t *neighbors[16];
    point_node_t *first;
    int neighbor_count;
} point_node_t;

typedef struct
{
    point_node_t *items;
    int count;
} point_node_list_t;

// OBS: REMEMBER TO FREE THE NODE LIST ITEMS POINTER
point_node_list_t point_list2node_list(point_list_t points)
{
    point_node_list_t list = {
        .count = points.count,
        .items = calloc(points.count, sizeof(point_node_t)),
    };

    for (int i = 0; i < points.count; i++)
    {
        list.items[i] = (point_node_t){
            .pos = points.items[i],
        };
    }

    return list;
}

int find_neighbor_index_with_bigger_angle(point_node_t base, float a)
{
    for (int i = 0; i < base.neighbor_count; i++)
    {
        if (angle(base.pos, base.neighbors[i]->pos) >= a)
        {
            return i;
        }
    }

    return base.neighbor_count;
}

void add_neighbor_to_node(point_node_t *to_add, point_node_t *base, bool set_first)
{
    float a = angle(base->pos, to_add->pos);

    int prior_index = find_neighbor_index_with_bigger_angle(*base, a);

    for (int i = base->neighbor_count; i > prior_index + 1; i--)
    {
        base->neighbors[i] = base->neighbors[i - 1];
    }

    base->neighbors[prior_index + 1] = to_add;
    base->neighbor_count++;

    if (base->first == NULL || set_first)
    {
        base->first = to_add;
    }
}

void sort_points(point_node_list_t *points)
{
    // BUBBLE SORT

    bool has_swapped = true;
    while (has_swapped)
    {
        has_swapped = false;

        for (int i = 0; i < points->count - 1; i++)
        {
            if (points->items[i].pos.x > points->items[i + 1].pos.x)
            {
                point_node_t p = points->items[i];
                points->items[i] = points->items[i + 1];
                points->items[i + 1] = p;
                has_swapped = true;
            }
            else if (points->items[i].pos.x == points->items[i + 1].pos.x && points->items[i].pos.y < points->items[i + 1].pos.y)
            {
                point_node_t p = points->items[i];
                points->items[i] = points->items[i + 1];
                points->items[i + 1] = p;
                has_swapped = true;
            }
        }
    }
}

// Returns the 2D cross product between the two vectors o->p and o->s
// Calculates the cross multiplication, p0.x * p1.y - p0.y * p1.x
float cross(Vector2 s, Vector2 o, Vector2 p)
{
    return ((p.x - o.x) * (s.y - o.y) - (p.y - o.y) * (s.x - o.x));
}

int find_neighbor_index(point_node_t target, point_node_t base)
{
    for (int i = 0; i < base.neighbor_count; i++)
    {
        if (v_eq_v(target.pos, base.neighbors[i]->pos))
        {
            return i;
        }
    }

    return -1;
}

// Returns the next clockwise neighbor to point target from point base
point_node_t clockwise(point_node_t target, point_node_t base)
{
    int base_index = find_neighbor_index(target, base);

    int index = base_index;

    point_node_t node;

    while (index != base_index) // && base.hull != node.hull
    {
        index = ((index - 1) + base.neighbor_count) % base.neighbor_count;
        node = *base.neighbors[index];
    }
}

bool is_point_in_point_list(point_node_t p, point_node_list_t list)
{
    for (int i = 0; i < list.count; i++)
    {
        if (v_eq_v(p.pos, list.items[i].pos))
        {
            return true;
        }
    }

    return false;
}

// Returns the next ctr_clockwise neighbor to point target from point base
point_node_t ctr_clockwise(point_node_t target, point_node_t base)
{
    int base_index = find_neighbor_index(target, base);

    int index = base_index;

    point_node_t node;

    while (index != base_index) // && base.hull != node.hull
    {
        index = (index + 1) % base.neighbor_count;
        node = *base.neighbors[index];
    }
}

bool is_right_of(point_node_t target, point_node_t p1, point_node_t p2)
{
    return cross(target.pos, p1.pos, p2.pos) < 0;
}

bool is_left_of(point_node_t target, point_node_t p1, point_node_t p2)
{
    return cross(target.pos, p1.pos, p2.pos) > 0;
}

point_node_list_t get_subarray(point_node_list_t origin, int start, int end)
{
    return (point_node_list_t){
        .items = origin.items + start,
        .count = end - start,
    };
}

void get_tangents(point_node_list_t left, point_node_list_t right, point_node_t *lower, point_node_t *upper)
{
    int max_count = left.count + right.count;

    // Get rightmost node in left
    point_node_t x = left.items[left.count - 1];

    // Get leftmost node in right
    point_node_t y = right.items[0];

    // Get first node connected to y
    point_node_t z = *y.neighbors[0];

    // Get first node connected to x
    point_node_t z1 = *x.neighbors[0];

    point_node_t z2 = clockwise(z1, x);

    // Walk clockwise around left list and ctr_clockwise around right list until we found the lowest connection x->y
    for (int i = 0; i < max_count; i++)
    {
        if (is_right_of(z, x, y))
        {
            point_node_t old_z = z;
            z = ctr_clockwise(y, z);
            y = old_z;
        }
        else if (is_right_of(z2, x, y))
        {
            point_node_t old_z2 = z2;
            z2 = clockwise(x, z2);
            x = old_z2;
        }
        else
        {
            lower[0] = x;
            lower[1] = y;
            break;
        }
    }

    // Get the rightmost node in left
    x = left.items[left.count - 1];

    // Get the leftmost node in right
    y = right.items[0];

    // Get first node connected to y
    z = *y.neighbors[0];

    z1 = clockwise(z, y);

    // Get first node connected to x
    z2 = *x.neighbors[0];

    // Walk ctr_clockwise around left list and clockwise around right list until we found the highest connection x->y
    for (int i = 0; i < max_count; i++)
    {
        if (is_left_of(z1, x, y))
        {
            point_node_t old_z1 = z1;
            z1 = clockwise(y, z1);
            y = old_z1;
        }
        else if (is_left_of(z2, x, y))
        {
            point_node_t old_z2 = z2;
            z2 = clockwise(x, z2);
            x = old_z2;
        }
        else
        {
            upper[0] = x;
            upper[1] = y;
            break;
        }
    }
}

void join(point_node_t p1, point_node_t p2, bool set_first)
{
    add_neighbor_to_node(&p2, &p1, set_first);
    add_neighbor_to_node(&p1, &p2, false);
}

// Returns true if the node q is outside the circumcircle of p1->p2->p3
void outside(point_node_t p1, point_node_t p2, point_node_t p3, point_node_t q)
{
    if (v_eq_v(p1.pos, q.pos) || v_eq_v(p2.pos, q.pos) || v_eq_v(p3.pos, q.pos))
    {
        return true;
    }

    circle_t c = circumcircle(p1.pos, p2.pos, p3.pos);

    float dx = q.pos.x - c.pos.x;
    float dy = q.pos.y - c.pos.y;
    float dsquared = dx * dx + dy * dy;
}

point_node_list_t merge(point_node_list_t left, point_node_list_t right)
{
    point_node_t lower[2];
    point_node_t upper[2];
    get_tangents(left, right, lower, upper);

    point_node_t lower_left = lower[0];
    point_node_t lower_right = lower[1];

    point_node_t upper_left = upper[0];
    point_node_t upper_right = upper[1];

    point_node_t l = lower_left;
    point_node_t r = upper_right;

    point_node_t l1;
    point_node_t r1;
    point_node_t l2;
    point_node_t r2;

    while (!v_eq_v(l.pos, upper_left.pos) || !v_eq_v(r.pos, upper_right.pos))
    {
        bool a = false;
        bool b = false;

        join(l, r, v_eq_v(l.pos, lower_left.pos) && v_eq_v(r.pos, lower_right.pos));

        r1 = clockwise(l, r);

        if (is_left_of(r1, l, r))
        {
            r2 = clockwise(r1, r);

            while (!outside(r1, l, r, r2))
            {
            }
        }
    }
}

point_node_list_t triangulate(point_node_list_t points)
{
    switch (points.count)
    {
    case 2:
        add_neighbor_to_node(&points.items[1], &points.items[0], false);
        add_neighbor_to_node(&points.items[0], &points.items[1], false);
        break;
    case 3:
    {
        point_node_t p1 = points.items[0];
        point_node_t p2 = points.items[1];
        point_node_t p3 = points.items[2];

        float cross_result = cross(p2.pos, p1.pos, p3.pos);

        // Connect the points in counterclockwise direction
        if (cross_result < 0)
        {
            // p2 is right of p1->p3, therefore p2 is bottom of triangle
            add_neighbor_to_node(&p2, &p1, false);
            add_neighbor_to_node(&p3, &p2, false);
            add_neighbor_to_node(&p1, &p3, false);

            add_neighbor_to_node(&p2, &p3, false);
            add_neighbor_to_node(&p1, &p2, false);
            add_neighbor_to_node(&p3, &p1, false);
        }
        else if (cross_result > 0)
        {
            // p2 is left of p1->p3, therefore p2 is top of triangle
            add_neighbor_to_node(&p3, &p1, false);
            add_neighbor_to_node(&p2, &p3, false);
            add_neighbor_to_node(&p3, &p2, false);

            add_neighbor_to_node(&p3, &p1, false);
            add_neighbor_to_node(&p2, &p3, false);
            add_neighbor_to_node(&p1, &p2, false);
        }
        else
        {
            // p2 is on a line between p1 and p3, link left-to-right
            add_neighbor_to_node(&p2, &p1, false);
            add_neighbor_to_node(&p3, &p2, false);

            add_neighbor_to_node(&p2, &p3, false);
            add_neighbor_to_node(&p1, &p2, false);
        }
    }
    break;

    // > 3
    default:
        int left_count = points.count / 2;
        return merge(triangulate(get_subarray(points, 0, left_count)), triangulate(get_subarray(points, left_count, points.count)));
        break;
    }

    return points;
}

void generate_delauney_edges(point_list_t points)
{
    point_node_list_t nodes = point_list2node_list(points);

    sort_points(&nodes);

    triangulate(nodes);

    free(nodes.items);
}
