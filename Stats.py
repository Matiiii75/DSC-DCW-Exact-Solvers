import os
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

# scientific article mode
sns.set_context("paper", font_scale=1) 

# Pure white background, subtle grid, sharp borders
sns.set_style("whitegrid", {
    "axes.edgecolor": "black",
    "axes.linewidth": 1.0,
    "grid.color": "lightgrey",
    "grid.linestyle": "--"
})

# Optimization of the rendering engine for LaTeX export (
plt.rcParams.update({
    "figure.dpi": 300,              
    "pdf.fonttype": 42,             
    "ps.fonttype": 42,
    "legend.frameon": True,         
    "legend.edgecolor": "black",
    "font.family": "serif",       
    "font.size": 8,
    "axes.labelsize": 8.5,
    "xtick.labelsize": 7.5,
    "ytick.labelsize": 7.5,
    "legend.fontsize": 7
})


def load_all_results(directory: str = "results") -> pd.DataFrame:
    fichiers = [f for f in os.listdir(directory) if f.startswith("results_") and f.endswith(".txt")]
    liste_df = []
    
    for fichier in fichiers:
        chemin = os.path.join(directory, fichier)
        algo_name = fichier.replace("results_", "").replace(".txt", "")
        
        if algo_name.startswith("MILP"):
            famille = "MILP"
            colonnes = ["inst", "n", "k", "val", "time", "gap", "inf_bound"]
            df = pd.read_csv(chemin, sep=r"\s+", names=colonnes, header=None, usecols=range(7))
            
        elif algo_name.startswith("FPT_PT"):
            famille = "FPT_PT"
            colonnes = ["inst", "n", "k", "val", "nb_cands", "time", "nb_comp", "nb_sub_pb"]
            df = pd.read_csv(chemin, sep=r"\s+", names=colonnes, header=None)
            
        elif algo_name.startswith("FPT"):
            famille = "FPT"
            colonnes = ["inst", "n", "k", "val", "nb_cands", "time"]
            df = pd.read_csv(chemin, sep=r"\s+", names=colonnes, header=None)
            
        else:
            continue
            
        df["algorithme"] = algo_name
        df["famille"] = famille
        liste_df.append(df)
        
    df_totale = pd.concat(liste_df, ignore_index=True)
    return df_totale


def pad_missing_instances(df: pd.DataFrame, time_limit: float = 600.0) -> pd.DataFrame:
    df_commun = df[df["n"] <= 500].copy()
    df_exclusif = df[df["n"] > 500].copy()
    
    instances_uniques = df_commun[['inst', 'n', 'k']].drop_duplicates()
    algos_uniques = df_commun[['algorithme', 'famille']].drop_duplicates()
    
    grille_complete = instances_uniques.merge(algos_uniques, how='cross')
    
    df_commun_padded = grille_complete.merge(
        df_commun, 
        on=['inst', 'n', 'k', 'algorithme', 'famille'], 
        how='left'
    )
    df_commun_padded['time'] = df_commun_padded['time'].fillna(time_limit + 1.0)
    
    df_finale = pd.concat([df_commun_padded, df_exclusif], ignore_index=True)
    return df_finale


def compare_temps_cotes_a_cotes(df_propre: pd.DataFrame): 
    df_base = df_propre[df_propre["n"] <= 250].copy()
    
    df_dsc = df_base[df_base["algorithme"].isin(["FPT_DSC", "MILP_DSC1", "MILP_DSC2", "MILP_DSC3"])]
    df_cw = df_base[df_base["algorithme"].isin(["FPT_DCW", "MILP_DCW2", "MILP_DCW3"])]

    couleurs_unifiees = {
        "FPT_DSC": "blue",    
        "FPT_DCW": "blue",     
        "MILP_DSC1": "red",   
        "MILP_DSC2": "green", 
        "MILP_DCW2": "green",  
        "MILP_DSC3": "orange",
        "MILP_DCW3": "orange"  
    }

    fig, axes = plt.subplots(1, 2, figsize=(5.2, 2.8), sharey=True)

    sns.lineplot(
        data=df_dsc, x="k", y="time", hue="algorithme", 
        palette=couleurs_unifiees, marker="o", errorbar=None, ax=axes[0] 
    )
    axes[0].set_xlabel("Degeneracy")
    axes[0].set_ylabel("Time (s)")
    axes[0].grid(axis='y', linestyle='--', alpha=0.4)
    axes[0].grid(axis='x', linestyle='--', alpha=0.4)
    axes[0].legend(title="DSC algorithms", loc="best", fontsize=6, title_fontsize=7, labelspacing=0.2, borderpad=0.2)

    sns.lineplot(
        data=df_cw, x="k", y="time", hue="algorithme", 
        palette=couleurs_unifiees, marker="o", errorbar=None, ax=axes[1] 
    )
    axes[1].set_xlabel("Degeneracy")
    axes[1].set_ylabel("") 
    axes[1].grid(axis='y', linestyle='--', alpha=0.4)
    axes[1].grid(axis='x', linestyle='--', alpha=0.4)
    axes[1].legend(title="DCW algorithms", loc="best", fontsize=6, title_fontsize=7, labelspacing=0.2, borderpad=0.2)

    plt.tight_layout() 
    plt.savefig("graphs/compare_time_k_FPT.pdf", format="pdf", bbox_inches="tight")
    plt.close()


def graphique_temps_n_k_FPT(df): 
    df_filtree = df[df["famille"] == "FPT"].copy()
    df_filtree = df_filtree[df_filtree["time"] < 600.00] 

    df_filtree = df_filtree.rename(columns={"k": r"$\delta$", "algorithme": "Algorithm"})
    df_filtree[r"$\delta$"] = df_filtree[r"$\delta$"].astype(str)
    
    df_filtree = df_filtree.sort_values(by=[r"$\delta$", "n"], key=lambda x: x.astype(int) if x.name == r"$\delta$" else x)

    plt.figure(figsize=(4.0, 2.8))

    sns.lineplot(
        data=df_filtree, x="n", y="time", hue=r"$\delta$", 
        style="Algorithm", markers=True, dashes=True, errorbar=None
    )

    plt.yscale("log")
    
    # --- CORRECTION 1: Power 0.7 to reduce the excessive spacing of small instances ---
    power = 0.7
    forward = lambda x: np.power(np.abs(x), power)
    inverse = lambda x: np.power(np.abs(x), 1/power)
    plt.xscale('function', functions=(forward, inverse))
    
    # --- CORRECTION 2: Manual limits to detach the first point (n=50) from the Y axis ---
    plt.xlim(10, 2100)
    
    # Force text display
    ticks = [50, 100, 150, 250, 500, 1000, 2000]
    plt.xticks(ticks, [str(t) for t in ticks], rotation=45, ha="right", rotation_mode="anchor")

    plt.grid(axis='y', linestyle='--', alpha=0.4)
    plt.grid(axis='x', linestyle='--', alpha=0.4)
    plt.xlabel("Dag size")
    plt.ylabel("Time (s)")

    plt.legend(
        title="Parameters",
        # loc="upper left",
        ncol=2,
        fontsize=5,             
        title_fontsize=6,      
        markerscale=1.0,        
        labelspacing=0.1,       
        handletextpad=0.3,      
        borderpad=0.2,
        columnspacing=0.8,
        handlelength=3.5
    )
    
    plt.savefig("graphs/time_n_k_FPT.pdf", format="pdf", bbox_inches="tight")
    plt.tight_layout()
    plt.close()


def plot_heatmaps_resolution_fpt_classique(df: pd.DataFrame, time_limit: float = 600.0):
    df_work = df.copy()
    df_work["solved"] = df_work["time"] < time_limit
    
    df_dsc = df_work[df_work["algorithme"] == "FPT_DSC"]
    df_cw = df_work[df_work["algorithme"] == "FPT_DCW"]
    
    pivot_dsc = pd.pivot_table(df_dsc, values="solved", index="k", columns="n", aggfunc="mean") * 100
    pivot_cw = pd.pivot_table(df_cw, values="solved", index="k", columns="n", aggfunc="mean") * 100
    
    pivot_dsc = pivot_dsc.sort_index(ascending=False)
    pivot_cw = pivot_cw.sort_index(ascending=False)

    fig, axes = plt.subplots(1, 2, figsize=(5.5, 3.2), sharey=True)
    
    heatmap_kwargs = {
        "annot": True,        
        "fmt": ".0f",         
        "cmap": "RdYlGn",     
        "vmin": 0,            
        "vmax": 100,          
        "linewidths": 0.5,
        "annot_kws": {"size": 5.5}
    }

    sns.heatmap(pivot_dsc, ax=axes[0], cbar=False, **heatmap_kwargs)
    axes[0].set_xlabel("Dag size")
    axes[0].set_ylabel("Degeneracy")

    sns.heatmap(pivot_cw, ax=axes[1], cbar=False, **heatmap_kwargs)
    axes[1].set_xlabel("Dag size")
    axes[1].set_ylabel("")
    axes[1].tick_params(labelleft=True)
    
    for ax in axes:
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right", rotation_mode="anchor")
        plt.setp(ax.get_yticklabels(), rotation=0)

    mappable = axes[0].collections[0]
    
    # Color bar adjusted to be smaller and thinner
    cbar = fig.colorbar(
        mappable, 
        ax=axes.ravel().tolist(), 
        orientation="horizontal", 
        pad=0.3,       
        shrink=0.5,    # Reduces the width of the bar (was at 0.7)
        aspect=30      # Makes the bar thinner/less thick (default often at 20)
    )
    
    # Shortened text and reduced font size for the legend
    cbar.set_label("Solve rate (%)", size=8)
    
    # Optional: also reduce the size of the numbers (0, 20, 40...) on the bar
    cbar.ax.tick_params(labelsize=7) 

    plt.savefig("graphs/heatmap_FPT.pdf", format="pdf", bbox_inches="tight")
    plt.close()


def plot_evolution_taux_resolution_cotes_a_cotes(df: pd.DataFrame, time_limit: float = 600.0):
    df_plot = df[df["n"] <= 500].copy()
    df_plot["solved"] = df_plot["time"] < time_limit
    
    df_stats = df_plot.groupby(["n", "algorithme"])["solved"].mean().reset_index()
    df_stats["solved_pct"] = df_stats["solved"] * 100
    
    df_stats["n"] = df_stats["n"].astype(int)
    df_stats = df_stats.sort_values(by="n")
    df_stats["n"] = df_stats["n"].astype(str)

    algos_dsc = ["FPT_DSC", "FPT_PT_DSC", "MILP_DSC1", "MILP_DSC2", "MILP_DSC3"]
    algos_cw = ["FPT_DCW", "FPT_PT_DCW", "MILP_DCW2", "MILP_DCW3"]
    
    df_dsc = df_stats[df_stats["algorithme"].isin(algos_dsc)]
    df_cw = df_stats[df_stats["algorithme"].isin(algos_cw)]

    couleurs_unifiees = {
        "FPT_DSC": "#1f77b4",     
        "FPT_DCW": "#1f77b4",      
        "FPT_PT_DSC": "#9467bd",  
        "FPT_PT_DCW": "#9467bd",   
        "MILP_DSC1": "#d62728",   
        "MILP_DSC2": "#2ca02c",   
        "MILP_DCW2": "#2ca02c",    
        "MILP_DSC3": "#ff7f0e",   
        "MILP_DCW3": "#ff7f0e"     
    }

    fig, axes = plt.subplots(1, 2, figsize=(5.2, 2.8), sharey=True)

    sns.lineplot(
        data=df_dsc, x="n", y="solved_pct", hue="algorithme",
        palette=couleurs_unifiees, marker="o", markersize=4,
        dashes=False, linewidth=1.2, ax=axes[0]
    )
    axes[0].set_xlabel("Dag size")
    axes[0].set_ylabel("Solve rate (%)")
    axes[0].set_ylim(-5, 105)
    axes[0].grid(axis='y', linestyle='--', alpha=0.4)
    axes[0].grid(axis='x', linestyle='--', alpha=0.4)
    axes[0].legend(title="DSC algorithms", loc="best", fontsize=5, title_fontsize=6, markerscale=1.0, labelspacing=0.1, borderpad=0.2)

    sns.lineplot(
        data=df_cw, x="n", y="solved_pct", hue="algorithme",
        palette=couleurs_unifiees, marker="o", markersize=4,
        dashes=False, linewidth=1.2, ax=axes[1]
    )
    axes[1].set_xlabel("Dag size")
    axes[1].set_ylabel("")  
    axes[1].grid(axis='y', linestyle='--', alpha=0.4)
    axes[1].grid(axis='x', linestyle='--', alpha=0.4)
    axes[1].legend(title="DCW algorithms", loc="best", fontsize=5, title_fontsize=6, markerscale=1.0, labelspacing=0.1, borderpad=0.2)

    plt.tight_layout()
    plt.savefig("graphs/solve_rate_all_algos.pdf", format="pdf", bbox_inches="tight")
    plt.close()


def plot_impact_pretraitement_temps_candidats(df_propre: pd.DataFrame):
    df_plot = df_propre[
        (df_propre["algorithme"].isin(["FPT_DSC", "FPT_PT_DSC"])) & 
        (df_propre["n"].isin([50, 100, 1000]))
    ].copy()

    df_plot = df_plot.rename(columns={"n" : "Dag size", "algorithme": "Algorithm"})
    df_plot["Dag size"] = df_plot["Dag size"].astype(int).astype(str)
    df_plot = df_plot[df_plot["time"] <= 600.00]
    df_plot = df_plot.sort_values(by=["Dag size", "k"], key=lambda x: x.astype(int) if x.name in ["Dag size", "k"] else x)
    
    palette_n = {"50": "red", "100": "blue", "1000": "green"}
    fig, axes = plt.subplots(1, 2, figsize=(5.2, 2.8))
    
    sns.lineplot(
        data=df_plot, x="k", y="time", hue="Dag size", style="Algorithm",
        palette=palette_n, markers=["o", "s"], dashes=True, errorbar=None, ax=axes[0]
    )
    axes[0].set_title("Average solve time", pad=10)
    axes[0].set_xlabel("Degeneracy")
    axes[0].set_ylabel("Time (s)")
    axes[0].grid(axis='y', linestyle='--', alpha=0.4)
    axes[0].grid(axis='x', linestyle='--', alpha=0.4)
    axes[0].legend(loc="best", fontsize=5, title_fontsize=6, markerscale=1.0, labelspacing=0.1, borderpad=0.2)

    sns.lineplot(
        data=df_plot, x="k", y="nb_cands", hue="Dag size", style="Algorithm",
        palette=palette_n, markers=["o", "s"], dashes=True, errorbar=None, ax=axes[1]
    )
    axes[1].set_title("Average state graph nodes generated", pad=10)
    axes[1].set_xlabel("Degeneracy")
    axes[1].set_ylabel("Number nodes created")
    # axes[1].set_yscale("log") 
    axes[1].grid(axis='y', linestyle='--', alpha=0.4)
    axes[1].grid(axis='x', linestyle='--', alpha=0.4)
    axes[1].legend(loc="best", fontsize=5, title_fontsize=6, markerscale=1.0, labelspacing=0.1, borderpad=0.2)

    plt.tight_layout()
    plt.savefig("graphs/FPT_vs_FPT-PT.pdf", format="pdf", bbox_inches="tight")
    plt.close()


def plot_gaps_timeouts_MILP(df: pd.DataFrame, time_limit: float = 600.0):
    df_milp = df[(df["famille"] == "MILP") & (df["time"] >= time_limit)].copy()
    
    df_milp["gap"] = pd.to_numeric(df_milp["gap"], errors="coerce")
    df_milp.loc[df_milp["gap"] > 10.0, "gap"] = float('nan')
    df_milp = df_milp.dropna(subset=["gap"])
    df_milp["gap_pct"] = df_milp["gap"] * 100
    
    # 1. Define explicit order to group DSC on the left and DCW on