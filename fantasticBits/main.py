
#!/usr/bin/python
#encoding: utf8

import sys
import math

class Snaffles:
    def __init__(self, x, y, vx, vy, id_entity):
        self._x = x
        self._y = y
        self._vx = vx
        self._vy = vy
        self._id = id_entity
        self._type = "SNAFFLE"
        self._is_taken = False
    @property
    def x(self):
        return self._x
    @property
    def y(self):
        return self._y
    @property
    def vx(self):
        return self._vx
    @property
    def vy(self):
        return self._vy
    def update(self, x, y, vx, vy):
        self._x = x
        self._y = y
        self._vx = vx
        self._vy = vy

class Wizard:
    def __init__(self, x, y, vx, vy, id_entity, list_snaffles, myteamid, compteur,\
    cd_obliviate, cd_petrificus, cd_accio, cd_flipendo, list_bludgers, list_opponents, state=0):
        self._state = state
        self.team = myteamid
        self._x = x
        self._y = y
        self._vx = vx
        self._vy = vy
        self._id = id_entity
        self._type = "WIZARD"
        self._mana = compteur
        self._list_snaffles = list_snaffles
        self.list_bludgers = list_bludgers
        self.list_opponents = list_opponents
        self._list_cd = [cd_obliviate, cd_petrificus, cd_accio, cd_flipendo]
    @property
    def x(self):
        return self._x
    @property
    def y(self):
        return self._y
    @property
    def vx(self):
            return self._vx
    @property
    def vy(self):
        return self._vy
    @property
    def state(self):
        return self._state
    @property
    def list_snaffles(self):
        return self._list_snaffles

    def update_snaffles(self, list_snaffles):
        self._list_snaffles = list_snaffles

    def update(self, x, y, vx, vy, state):
        self._x = x
        self._y = y
        self._vx = vx
        self._vy = vy
        self._state = state

    def find_snaffle(self):
        dist = 100000000
        nearest_snaffle = 0
        for snaffle in self.list_snaffles:
            if not snaffle._is_taken:
                dist2 = (self.x-snaffle.x)**2+(self.y-snaffle.y)**2
                if dist2 <= dist:
                    dist = dist2
                    nearest_snaffle = self.list_snaffles.index(snaffle)
        return nearest_snaffle

    def get_cooldowns(self):
        return self._list_cd

    def set_cooldowns(self, lcd):
        self._list_cd = lcd

    def nearest_oftwo(self, l):
        dist = (l[0].x-self.x)**2+(l[0].y-self.y)**2
        dist2 = (l[1].x-self.x)**2+(l[1].y-self.y)**2
        if dist <= dist2:
            return 0,dist
        return 1,dist2

    def dist_snaffle(self, snaffle):
        return (self.x-snaffle.x)**2+(self.y-snaffle.y)**2

    def snaffle_accio(self):
        best_accio = 0
        dist_snaffle = 10000
        if self.team==0:
            for snaffle in self.list_snaffles:
                tmp = self.dist_snaffle(snaffle)
                if (snaffle.x - self.x < 0 and tmp < 5000**2 and tmp > 2000**2) or best_accio == 0:
                    best_accio = snaffle
                    dist_snaffle = tmp
        else:
            for snaffle in self.list_snaffles:
                tmp = self.dist_snaffle(snaffle)
                if (snaffle.x - self.x > 0 and tmp < 6000**2 and tmp > 2000**2) or best_accio == 0:
                    best_accio = snaffle
                    dist_snaffle = tmp
        return best_accio

    def nearest_snaffle(self):
        dist = 10000000000
        near_snaffle = 0
        for i in range(1, len(self.list_snaffles)):
            dist2 = (self.list_snaffles[i].x-self.x)**2+(self.list_snaffles[i].y-self.y)**2
            if dist2 <= dist:
                dist = dist2
                near_snaffle = i
        return near_snaffle, dist

    def move(self):
        for i in range(len(self._list_cd)):
                if self._list_cd[i] != 0:
                    self._list_cd[i] -= 1
        choice_pos = self.find_snaffle()
        new_x = self.list_snaffles[choice_pos].x
        new_y = self.list_snaffles[choice_pos].y
        print("MOVE "+ str(new_x) + " "+ str(new_y) + " 150")
        self._x = new_x
        self._y = new_y

    def throw(self):
        for i in range(len(self._list_cd)):
                if self._list_cd[i] != 0:
                    self._list_cd[i] -= 1
        if self.team == 0:
            print("THROW 16000 3600 500")
        if self.team == 1:
            print("THROW 0 3600 500")

    def accio(self):
        for i in range(len(self._list_cd)):
                if self._list_cd[i] != 0 and i != 2:
                    self._list_cd[i] -= 1
        dist_snaffle = self.nearest_snaffle()[1]
        if self._mana > 20 and dist_snaffle < 5000**2 and dist_snaffle > 2000**2 and self._list_cd[2] == 0:
            print("ACCIO "+str(self.list_snaffles[self.nearest_snaffle()[0]]._id))
            self._list_cd[2] = 6
            return True
        return False

    def accio2(self):
        for i in range(len(self._list_cd)):
                if self._list_cd[i] != 0 and i != 2:
                    self._list_cd[i] -= 1
        snaffle = self.snaffle_accio()
        if self._mana > 20 and self._list_cd[2] == 0:
            print("ACCIO "+str(snaffle._id))
            self._list_cd[2] = 6
            return True
        return False

    def obstacle_intrajectory(self, a, b):
        for bludger in self.list_bludgers:
            x = bludger.x
            if abs(bludger.y-a*x-b) <= 200:
                return True
        for o in self.list_opponents:
            x = o.x
            if abs(o.y-a*x-b) <= 400:
                return True
        return False

    def flipendo(self, elt_type):
        if elt_type == "OPPONENT_WIZARD":
            snaffle_in_view, dist_snaffle = self.nearest_snaffle()
            snaffle = self.list_snaffles[snaffle_in_view]
            for o in self.list_opponents:
                if (o.x-snaffle.x)**2+(o.y-snaffle.y)**2 <= (self.x - snaffle.x)**2 + (self.y - snaffle.y)**2\
                and (o.x-self.x)**2 + (o.y-self.y)**2 <= 4000**2:
                     if self._mana > 20 and self._list_cd[3] == 0:
                        print("FLIPENDO "+str(o._id))
                        self._list_cd[3] = 3
                        return True
        if elt_type == "BLUDGER":
            for bludger in self.list_bludgers:
                if (self.x-bludger.x)**2+(self.y-bludger.y)**2 <=500**2:
                     if self._mana > 20 and self._list_cd[3] == 0:
                        print("FLIPENDO "+str(bludger._id))
                        self._list_cd[3] = 3
                        return True
        if elt_type == "SNAFFLE":
            snaffle_in_view, dist_snaffle = self.nearest_snaffle()
            if self._mana > 20 and dist_snaffle < 10000**2 and self._list_cd[3] == 0:
                snaffle = self.list_snaffles[snaffle_in_view]
                if snaffle.x != self.x:
                    a_line = (snaffle.y - self.y)/(snaffle.x - self.x)
                    b_line = self.y - a_line * self.x
                    if self.team == 0:
                        x_line = 16000
                        y_line = a_line*x_line+b_line
                        if y_line>= 3500 and y_line <= 3900:
                            if not self.obstacle_intrajectory(a_line, b_line):
                                print("FLIPENDO "+str(snaffle._id))
                                self._list_cd[3] = 3
                                return True
                            else:
                                return False
                    if self.team == 1:
                        x_line = 0
                        y_line = a_line*x_line+b_line
                        if y_line>= 3500 and y_line <= 3900:
                             if not self.obstacle_intrajectory(a_line, b_line):
                                print("FLIPENDO "+str(snaffle._id))
                                self._list_cd[3] = 3
                                return True
                             else:
                                return False
                else:
                    return False

    def petrificus(self):
        dist_bludger = self.nearest_oftwo(self.list_bludgers)[1]
        if self._mana > 10 and dist_bludger < 700**2 and dist_bludger > 0 and self._list_cd[1] == 0:
            print("PETRIFICUS "+str(self.list_bludgers[self.nearest_oftwo(self.list_bludgers)[0]]._id))
            self._list_cd[1] = 1
            return True

    def obliviate(self):
        dist_bludger = self.nearest_oftwo(self.list_bludgers)[1]
        if self._mana > 5 and dist_bludger < 2000**2 and dist_bludger > 0 and self._list_cd[0] == 0:
            print("OBLIVIATE "+str(self.list_bludgers[self.nearest_oftwo(self.list_bludgers)[0]]._id))
            self._list_cd[0] = 3
            return True

    def fight_for_last(self):
        if len(self.list_snaffles) == 1:
            if self.state == 1:
                f = self.flipendo("SNAFFLE")
                if f :
                    return True
            else:
                a = self.flipendo("OPPONENT_WIZARD")
                if a:
                    return True
        return False

    def action(self):
        c = self.flipendo("SNAFFLE")
        if not c:
        #fftl = self.fight_for_last()
        #if fftl:
        #    return 0
            a = self.accio()
            if a:
                return 0
        else:
           return 0
        #b = self.petrificus()
        #d = self.obliviate()
        #if (c):
        #    return 0
        if self.state == 0:
            self.move()
            return 0
        if self.state == 1:
            self.throw()
            return 0

    def defend(self):
        return 0

class Bludger:
    def __init__(self, x, y, vx, vy, entity_id, list_wizards):
        self._x = x
        self._y = y
        self._vx = vx
        self._vy = vy
        self._id = entity_id
        self._type = "BLUDGER"
        self.list_wizards = list_wizards
    @property
    def x(self):
        return self._x
    @property
    def y(self):
        return self._y
    @property
    def vx(self):
        return self._vx
    @property
    def vy(self):
        return self._vy
    def nearest_wizard(self):
        """ Returns the ID of the aimed wizard """
        dist = (self.list_wizards[0].x-self.x)**2+(self.list_wizards[0].y-self.y)**2
        near_wizard = 0
        for i in range(1, len(self.list_wizards)):
            dist2 = (self.list_wizards[i].x-self.x)**2+(self.list_wizards[i].y-self.y)**2
            if dist2 <= dist:
                dist = dist2
                near_wizard = i
        return self.list_wizards[near_wizard]._id

class Opponent:
    def __init__(self, x, y, vx, vy, entity_id, list_snaffles, state):
        self._x = x
        self._state = state
        self._y = y
        self._vx = vx
        self._vy = vy
        self._id = entity_id
        self._type = "OPPONENT_WIZARD"
        self.list_snaffles = list_snaffles
    @property
    def x(self):
        return self._x
    @property
    def y(self):
        return self._y
    @property
    def vx(self):
        return self._vx
    @property
    def vy(self):
        return self._vy

    def find_snaffle(self):
        dist = 100000000
        nearest_snaffle = 0
        for snaffle in self.list_snaffles:
            if not snaffle._is_taken:
                dist2 = (self.x-snaffle.x)**2+(self.y-snaffle.y)**2
                if dist2 <= dist:
                    dist = dist2
                    nearest_snaffle = self.list_snaffles.index(snaffle)
        return nearest_snaffle

    def nearest_snaffle(self):
        dist = (self.list_snaffles[0].x-self.x)**2+(self.list_snaffles[0].y-self.y)**2
        near_snaffle = 0
        for i in range(1, len(self.list_snaffles)):
            dist2 = (self.list_snaffles[i].x-self.x)**2+(self.list_snaffles[i].y-self.y)**2
            if dist2 <= dist:
                dist = dist2
                near_snaffle = i
        return near_snaffle, dist

# Grab Snaffles and try to throw them through the opponent's goal!
# Move towards a Snaffle and use your team id to determine where you need to throw it.

my_team_id = int(input())  # if 0 you need to score on the right of the map, if 1 you need to score on the left

# game loop
mana = 0
cd_obliviate, cd_petrificus, cd_accio, cd_flipendo = 0, 0, 0, 0
while True:
    list_snaffles = []
    list_wizards = []
    list_bludgers = []
    list_opponents = []
    mana += 1
    score, magic = [int(i) for i in input().split()]
    score, magic = [int(i) for i in input().split()]
    entities = int(input())
    for i in range(entities):
        # entity_id: entity identifier
        # entity_type: "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
        # x: position
        # y: position
        # vx: velocity
        # vy: velocity
        # state: 1 if the wizard is holding a Snaffle, 0 otherwise
        entity_id, entity_type, x, y, vx, vy, state = input().split()
        entity_id = int(entity_id)
        x = int(x)
        y = int(y)
        vx = int(vx)
        vy = int(vy)
        state = int(state)
        if entity_type == "SNAFFLE":
            list_snaffles.append(Snaffles(x, y, vx, vy, entity_id))
        if entity_type == "BLUDGER":
            list_bludgers.append(Bludger(x, y, vx, vy, entity_id, list_wizards))
        if entity_type == "OPPONENT_WIZARD":
            list_opponents.append(Opponent(x, y, vx, vy, entity_id, list_snaffles, state))
        if entity_type == "WIZARD":
            list_wizards.append(Wizard(x, y, vx, vy, entity_id, list_snaffles, my_team_id, mana, \
            cd_obliviate, cd_petrificus, cd_accio, cd_flipendo, list_bludgers, list_opponents, state))

    i = list_wizards[0].find_snaffle()
    """#list_wizards[0].update_snaffles(list_snaffles)
    #del list_snaffles[i]
    #list_wizards[1].update_snaffles(list_snaffles)
    first = 0
    second = 1
    snaffle1 = list_snaffles[list_wizards[0].nearest_snaffle()[0]]
    snaffle2 = list_snaffles[list_wizards[1].nearest_snaffle()[0]]
    if snaffle1._id == snaffle2._id:
        if list_wizards[0].dist_snaffle(snaffle1) > list_wizards[1].dist_snaffle(snaffle1):
            first = 1
            second = 0
    l = list_snaffles
    del l[i]

    if first == 1:
        list_wizards[0].update_snaffles(l)
    if first == 0:
        list_wizards[1].update_snaffles(l)
        """
    list_wizards[0].action()
    cd_obliviate, cd_petrificus, cd_accio, cd_flipendo = list_wizards[0].get_cooldowns()
    if len(list_snaffles) > 1:
        del list_snaffles[i]
    list_wizards[1].update_snaffles(list_snaffles)
    list_wizards[1].set_cooldowns([cd_obliviate, cd_petrificus, cd_accio, cd_flipendo])
    list_wizards[1].action()

    cd_obliviate, cd_petrificus, cd_accio, cd_flipendo = list_wizards[1].get_cooldowns()

    """
    def ennemy_seen(self):
        dist_snaffle = self.nearest_snaffle()[1]
        snaffle_in_view = self.nearest_snaffle()[0]
        for opp in self.list_opponents:
            svo = opp.nearest_snaffle()[0]
            if svo == snaffle_in_view:
                return True, opp._id
        return False
        """

