

try:
    range = xrange
except NameError:
    pass

import random
from operator import itemgetter
_MAX_ITER = int(1e3)

# from constants import *
# random.seed(SEED)

class Medoid(object):
    __slots__ = ['kernel', 'elements']

    def __init__(self, kernel, elements=None):
        self.kernel = kernel
        self.elements = [] if elements is None else elements

    def __repr__(self):
        return 'Medoid({0}, {1})'.format(self.kernel, self.elements)

    def __iter__(self):
        return iter(self.elements)

    def compute_kernel(self, distance):
        return min(self, key=lambda e: sum(distance(e, other) for other in self))

    def compute_diameter(self, distance):
        return max(distance(a, b) for a in self for b in self)


def _k_medoids_spawn_once(points, k, distance, max_iterations=_MAX_ITER, verbose=True):

    if k <= 0:
        raise ValueError('Number of medoids must be strictly positive')
    if k > len(points):
        raise ValueError('Number of medoids exceeds number of points')

    # Medoids initialization
    medoids = [Medoid(kernel=p) for p in random.sample(points, k)]
    if verbose:
        print('* New chosen kernels: {0}'.format([m.kernel for m in medoids]))

    for n in range(1, 1 + max_iterations):
        
        for m in medoids:
            m.elements = []

        
        for p in points:
            closest_medoid = min(medoids, key=lambda m: distance(m.kernel, p))
            closest_medoid.elements.append(p)


        medoids = [m for m in medoids if m.elements]

        change = False
        for m in medoids:
            new_kernel = m.compute_kernel(distance)
            if new_kernel != m.kernel:
                m.kernel = new_kernel
                change = True

        if not change:
            break

    diameter = max(m.compute_diameter(distance) for m in medoids)
    if verbose:
        print('* Iteration over after {0} steps, max diameter {1}'.format(n, diameter))

    return diameter, medoids


def k_medoids(points, k, distance, spawn, max_iterations=_MAX_ITER, verbose=True):

    kw = {
        'points': points,
        'k': k,
        'distance': distance,
        'max_iterations': max_iterations,
        'verbose': verbose,
    }

    diameter, medoids = min((_k_medoids_spawn_once(**kw) for _ in range(spawn)), key=itemgetter(0))
    if verbose:
        print(('~~ Spawn end: min of max diameters {0:.3f} '
               'for medoids: {1}').format(diameter, medoids))

    return diameter, medoids


def k_medoids_auto_k(points, distance, spawn, diam_max, max_iterations=_MAX_ITER, verbose=True):

    if not points:
        raise ValueError('No points given!')

    kw = {
        'distance': distance,
        'spawn': spawn,
        'max_iterations': max_iterations,
        'verbose': verbose,
    }

    for k, _ in enumerate(points, start=1):
        diameter, medoids = k_medoids(points, k, **kw)
        if diameter <= diam_max:
            break
        if verbose:
            print('*** Diameter too big {0:.3f} > {1:.3f}'.format(diameter, diam_max))
            print('*** Now trying {0} clusters\n'.format(k + 1))

    if verbose:
        print('*** Diameter ok {0:.3f} <= {1:.3f}'.format(diameter, diam_max))
        print('*** Stopping, {0} clusters enough ({1} points initially)'.format(k, len(points)))

    return diameter, medoids
