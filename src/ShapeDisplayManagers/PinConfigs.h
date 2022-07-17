//
//  PinConfigs.h
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#ifndef PinConfigs_h
#define PinConfigs_h

// struct for easily passing around pin configs. accompanying == and != logic.
struct PinConfigs {
    double timeOfUpdate;
    float gainP;
    float gainI;
    int maxI;
    int deadZone;
    int maxSpeed;

    bool operator== (const PinConfigs &rhs) {
        return (gainP == rhs.gainP && gainI == rhs.gainI && maxI == rhs.maxI &&
                deadZone == rhs.deadZone && maxSpeed == rhs.maxSpeed);
    };
    bool operator!= (const PinConfigs &rhs) {
        return !(*this == rhs);
    };
};


#endif /* PinConfigs_h */
